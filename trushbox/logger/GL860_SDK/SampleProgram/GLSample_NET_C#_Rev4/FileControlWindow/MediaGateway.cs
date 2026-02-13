using GtcCommand;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace FileControlWindow
{
    public class DriveInformation
    {
        public virtual List<string> GetRootDrive()
        {
            return new List<string>(Environment.GetLogicalDrives());
        }
    }
    public class DriveInformationGL : DriveInformation
    {
        public GtcDevIo.DevIo DevIo { get; set; }

        public DriveInformationGL() { }
        public DriveInformationGL(GtcDevIo.DevIo obj = null) => DevIo = obj;
        public override List<string> GetRootDrive()
        {
            var group = new FileCommandGroup(DevIo);
            var ans = group.Dirve.ReadCommand();
            ans = ans.Replace("\"", "");
            var ss = ans.Split(' ');
            var ret = new List<string>();
            foreach (var d in ss.Select((item, index) => new { item, index }))
            {
                if (d.item.Length > 0)
                {
                    try { ret.Add($"\\{d.item.Substring(0, d.item.IndexOf(':'))}"); } catch { }
                }
            }
            return ret;
        }
    }

    public class DirectoryInformation
    {
        public string Name { get; set; }
        public DateTime LastAccessTime { get; set; }

        public long Length { get; set; }

        public DirectoryInformation() { }

        public virtual List<DirectoryInformation> GetDirectories(string filepath)
        {
            var dirList = new DirectoryInfo(filepath);
            var list = dirList.GetDirectories();
            List<DirectoryInformation> retlist = new List<DirectoryInformation>();
            foreach (var d in list)
            {
                var t = new DirectoryInformation();
                t.Name = d.Name;
                t.LastAccessTime = d.LastAccessTime;
                retlist.Add(t);
            }
            return retlist;
        }
    }

    public class DirectoryInformationGL : DirectoryInformation
    {
        public GtcDevIo.DevIo DevIo { get; set; }
        public DirectoryInformationGL(GtcDevIo.DevIo dev) : base() => DevIo = dev;

        public override List<DirectoryInformation> GetDirectories(string filepath)
        {
            List<DirectoryInformation> retlist = new List<DirectoryInformation>();
            new FileCommandGroup(DevIo).GetFileListQuery(filepath).ForEach(d =>
            {
                if (d.isdirectory == true)
                {
                    var t = new DirectoryInformationGL(DevIo);
                    t.Name = d.name;
                    t.Length = d.filesize;
                    t.LastAccessTime = DateTime.Parse($"{d.date} {d.time}");
                    retlist.Add(t);
                }
            });

            return retlist;
        }
    }
    public class FileInformation
    {
        string _filepath;
        public string Name { get; set; }
        public DateTime LastAccessTime { get; set; }
        public Int64 Length { get; set; }
        public FileInformation() { }

        public virtual void SetFileInformation(string filepath)
        {
            _filepath = filepath;
            try
            {
                FileInfo fileinfo = new FileInfo(filepath);
                this.Name = fileinfo.Name;
                this.LastAccessTime = fileinfo.LastAccessTime;
                this.Length = fileinfo.Length;
            }
            catch (IOException ie)
            {
                this.Name = ie.ToString();
            }
        }

        public virtual List<FileInformation> GetFiles(string filepath)
        {
            var list = new List<FileInformation>();
            Directory.GetFiles(filepath).ToList<string>().ForEach(d =>
            {
                var fi = new FileInformation();
                try
                {
                    FileInfo fileinfo = new FileInfo(filepath);
                    fi.Name = fileinfo.Name;
                    fi.LastAccessTime = fileinfo.LastAccessTime;
                    fi.Length = fileinfo.Length;
                }
                catch (IOException ie)
                {
                    fi.Name = ie.ToString();
                }
                list.Add(fi);
            });

            return list;
        }

    }

    public class FileInformationGL : FileInformation
    {
        public GtcDevIo.DevIo DevIo { get; set; }

        public FileInformationGL(GtcDevIo.DevIo dev) : base() => DevIo = dev;

        public override void SetFileInformation(string filepath)
        {
        }

        public override List<FileInformation> GetFiles(string filepath)
        {
            var list = new List<FileInformation>();
            new FileCommandGroup(DevIo).GetFileListQuery(filepath).ForEach(d =>
            {
                if (d.isdirectory == false)
                {
                    var fi = new FileInformation();
                    fi.Name = d.name;
                    fi.LastAccessTime = DateTime.Parse($"{d.date} {d.time}");
                    fi.Length = d.filesize;
                    list.Add(fi);
                }
            });

            return list;
        }
    }

    public class FileAction
    {
        public class ActionPart
        {
            public string name = "";
            public string filepath = "";
            public Int64 filesize = 0;
            public bool result = false;
        }

        public List<ActionPart> ActionList { get; set; } = new List<ActionPart>();

        public virtual void AddActionList(string name, string filepath, Int64 filesize = 0) => ActionList.Add(new ActionPart() { name = name, filepath = filepath, filesize = filesize });
        public virtual void ClearActionList() => ActionList.Clear();

        public virtual List<ActionPart> Exec(string destpath = "", Func<ProgressForm.UpdateParam, bool> update = null) { return ActionList; }
    }

    public class FileDownload : FileAction
    {
        public GtcDevIo.DevIo DevIo { get; set; }
        public FileDownload(GtcDevIo.DevIo dev) : base() => DevIo = dev;

        public override List<ActionPart> Exec(string destpath = "", Func<ProgressForm.UpdateParam, bool> update = null)
        {
            var param = new ProgressForm.UpdateParam();
            param.errorFlg = false;
            param.endFlg = false;
            param.totalFileBytes = 0;
            param.transFileBytes = 0;
            param.totalFileCount = ActionList.Count;
            param.fileCount = 0;
            param.startTime = DateTime.Now;
            if (update(param) == true)
            {
                param.errorFlg = true;
                goto EndFile;
            }

            param.totalAllBytes = ActionList.Select(x => x.filesize).Sum();

            foreach (var d in ActionList)
            {
                param.startFileTime = DateTime.Now;
                param.fileCount++;

                var group = new FileCommandGroup(DevIo);
                var trans = (FileCommandTrans)group.Trans;
                trans.Sour.SendCommand(d.filepath); // Set download file
                var ret = trans.Open.ReadCommand(); // Open file
                if (ret != "ok")
                {
                    param.errorFlg = true;
                    goto EndFile;
                }

                int size = 0;

                try { size = int.Parse(trans.Size.ReadCommand()); } catch { }   // Get file byte size
                if (size <= 0) goto CancelFile;

                param.totalFileBytes = size;
                param.transFileBytes = 0;
                if (update(param) == true) goto CancelFile;

                var outp = (FileCommandTransOutp)trans.Outp;
                Int64 start = 0;
                Int64 end = 0;
                const int packet = 65536;   // Number of bytes received at a time

                using (var writer = new BinaryWriter(new FileStream(Path.Combine(destpath, d.name), FileMode.Create)))  // Output file generation
                {
                    int retrycount = 0;
                    const int maxretrycount = 3;
                    while (end < size)  // Repeat until all 1 file data is read
                    {
                        start = end + 1;
                        end = (end + packet < size) ? end + packet : size;
                    Retry:
                        outp.SendCommand($"{start},{end}"); // Specify the number of start and end bytes to receive
                        var buf = outp.ReadBinary();    // Receive data
                        if (buf == null)
                        {
                            Console.WriteLine($"Buff is null");
                            if (retrycount > maxretrycount)
                            {
                                param.errorFlg = true;
                                goto CancelFile;
                            }
                            retrycount++;
                            goto Retry;
                        }
                        if (buf.Length <= 0)
                        {
                            Console.WriteLine($"Buff Length is 0 or less");
                            if (retrycount > maxretrycount)
                            {
                                param.errorFlg = true;
                                goto CancelFile;
                            }
                            retrycount++;
                            goto Retry;
                        }

                        writer.Write(buf);  // Export data
                        Console.WriteLine($"Start = {start}, End = {end}, ReadBuf = {buf.Length}");

                        param.transFileBytes += buf.Length;
                        param.transAllBytes += buf.Length;
                        if (update(param) == true) goto CancelFile;

                        retrycount = 0;
                    }
                }

                d.result = true;

            CancelFile:

                trans.Close.ReadCommand();

                if (update(param) == true) goto EndFile;
            }
        EndFile:
            param.endFlg = true;
            update(param);
            return ActionList;
        }
    }

    public class FileDelete : FileAction
    {
        public GtcDevIo.DevIo DevIo { get; set; }
        public FileDelete(GtcDevIo.DevIo dev) : base() => DevIo = dev;

        public override List<ActionPart> Exec(string destpath = "", Func<ProgressForm.UpdateParam, bool> update = null)
        {
            foreach (var d in ActionList)
            {
                var group = new FileCommandGroup(DevIo);
                var rm = (FileCommandRm)group.Rm;
                rm.SendCommand(d.filepath);

                d.result = true;

            }

            return ActionList;
        }
    }

    public class FolderDelete
    {
        public virtual bool Exec(string filepath)
        {
            try
            {
                Directory.Delete(filepath);
            }
            catch
            {
                return false;
            }
            return true;
        }
    }

    public class FolderDeleteGL : FolderDelete
    {
        public GtcDevIo.DevIo DevIo { get; set; }
        public FolderDeleteGL(GtcDevIo.DevIo dev = null) : base() => DevIo = dev;

        public override bool Exec(string filepath)
        {
            var group = new FileCommandGroup(DevIo);
            var rd = (FileCommandRd)group.Rd;
            rd.SendCommand(filepath);

            return true;
        }
    }


    public class CreateFolder
    {
        public virtual bool Exec(string filepath)
        {
            try
            {
                var ret = Directory.CreateDirectory(filepath);
            }
            catch
            {
            }

            return true;
        }
    }

    public class CreateFolderGL : CreateFolder
    {
        public GtcDevIo.DevIo DevIo { get; set; }

        public CreateFolderGL(GtcDevIo.DevIo dev = null) : base() => DevIo = dev;

        public override bool Exec(string filepath)
        {
            try
            {
                var group = new FileCommandGroup(DevIo);
                var md = (FileCommandMd)group.Md;
                md.SendCommand(filepath);
            }
            catch
            {
            }

            return true;
        }
    }

    public class SelectFilePath
    {
        public virtual bool Exec(string filepath)
        {
            return true;
        }
    }

    public class SelectFilePathGL : SelectFilePath
    {
        public GtcDevIo.DevIo DevIo { get; set; }

        public SelectFilePathGL(GtcDevIo.DevIo dev = null) : base() => DevIo = dev;

        public override bool Exec(string filepath)
        {
            var group = new DataCommandGroup(DevIo);
            var capt = (DataCommandCapt)group.Capt;
            capt.SendCommand(filepath);
            var ret = capt.ReadCommand();
            return ret.Contains(filepath);
        }
    }

    public class SaveCND
    {
        public virtual bool Exec(string filepath)
        {
            return true;
        }
    }

    public class SaveCNDGL : SaveCND
    {
        public GtcDevIo.DevIo DevIo { get; set; }

        public SaveCNDGL(GtcDevIo.DevIo dev = null) : base() => DevIo = dev;

        public override bool Exec(string filepath)
        {
            try
            {
                var group = new FileCommandGroup(DevIo);
                var save = (FileCommandSave)group.Save;
                save.SendCommand(filepath);
            }
            catch
            {
            }

            return true;
        }
    }

    public class LoadCND
    {
        public virtual bool Exec(string filepath)
        {
            return true;
        }
    }

    public class LoadCNDGL : LoadCND
    {
        public GtcDevIo.DevIo DevIo { get; set; }

        public LoadCNDGL(GtcDevIo.DevIo dev = null) : base() => DevIo = dev;

        public override bool Exec(string filepath)
        {
            try
            {
                var group = new FileCommandGroup(DevIo);
                var load = (FileCommandLoad)group.Load;
                load.SendCommand(filepath);
            }
            catch
            {
            }

            return true;
        }
    }
}
