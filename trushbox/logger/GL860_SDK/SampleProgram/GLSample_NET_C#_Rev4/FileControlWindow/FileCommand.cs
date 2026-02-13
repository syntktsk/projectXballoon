using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace FileControlWindow
{
    class FileCommandGroup
    {
        public FileCommand Save { get; set; } = new FileCommandSave();
        public FileCommand Load { get; set; } = new FileCommandLoad();
        public FileCommand Dirve { get; set; } = new FileCommandDrive();
        public FileCommand Cd { get; set; } = new FileCommandCd();
        public FileCommand Rm { get; set; } = new FileCommandRm();
        public FileCommand Rd { get; set; } = new FileCommandRd();
        public FileCommand Md { get; set; } = new FileCommandMd();
        public FileCommand ListForm { get; set; } = new FileCommandListForm();
        public FileCommand ListQuery { get; set; } = new FileCommandListQuery();

        public FileCommand Trans { get; set; }

        public FileCommandGroup(GtcDevIo.DevIo deviceManager)
        {
            Save.DevIo = deviceManager;
            Load.DevIo = deviceManager;
            Dirve.DevIo = deviceManager;
            Cd.DevIo = deviceManager;
            Rm.DevIo = deviceManager;
            Rd.DevIo = deviceManager;
            Md.DevIo = deviceManager;
            ListForm.DevIo = deviceManager;
            ListQuery.DevIo = deviceManager;
            Trans = new FileCommandTrans(deviceManager);
        }

        public List<FileCommandListQuery.DirectroyPart> GetFileListQuery(string filepath)
        {
            this.Cd.SendCommand(filepath);
            var ret = Cd.ReadCommand();
            if (ret.Contains(filepath) == false) return new List<FileCommandListQuery.DirectroyPart>();
            this.ListForm.SendCommand("LONG");
            return ((FileCommandListQuery)this.ListQuery).ReadCommandList();
        }
    }

    class FileCommand
    {
        internal GtcDevIo.DevIo DevIo { get; set; }

        internal virtual string CommandSend { get; set; } = "";
        internal virtual string CommandRead { get; set; } = "";

        public virtual void SendCommand(string str = null)
        {
            DevIo?.SendQuery(string.Format(CommandSend, str));
        }

        public virtual string ReadCommand(string str = null)
        {
			DevIo?.Clean();  // Garbage removal processing
            var ret = DevIo?.SendQuery(string.Format(CommandRead));
            ret = GtcDevIo.DevIo.CutStringSpace(ret);
            return ret;
        }
    }

    class FileCommandSave : FileCommand
    {
        internal override string CommandSend { get; set; } = ":FILE:SAVE \"{0}\"";
        internal override string CommandRead { get; set; } = "";
    }

    class FileCommandLoad : FileCommand
    {
        internal override string CommandSend { get; set; } = ":FILE:LOAD \"{0}\"";
        internal override string CommandRead { get; set; } = "";
    }

    class FileCommandDrive : FileCommand
    {
        internal override string CommandSend { get; set; } = "";
        internal override string CommandRead { get; set; } = ":FILE:DRIVE?";
	}

    class FileCommandCd : FileCommand
    {
        internal override string CommandSend { get; set; } = ":FILE:CD \"{0}\"";
        internal override string CommandRead { get; set; } = ":FILE:CD?";
    }

    class FileCommandRm : FileCommand
    {
        internal override string CommandSend { get; set; } = ":FILE:RM \"{0}\"";
        internal override string CommandRead { get; set; } = "";
    }

    class FileCommandRd : FileCommand
    {
        internal override string CommandSend { get; set; } = ":FILE:RD \"{0}\"";
        internal override string CommandRead { get; set; } = "";
    }

    class FileCommandMd : FileCommand
    {
        internal override string CommandSend { get; set; } = ":FILE:MD \"{0}\"";
        internal override string CommandRead { get; set; } = "";
    }

    class FileCommandListForm : FileCommand
    {
        internal override string CommandSend { get; set; } = ":FILE:LIST:FORM {0}";
        internal override string CommandRead { get; set; } = ":FILE:LIST:FORM?";
    }

	class FileCommandListQuery : FileCommand
	{
		internal override string CommandSend { get; set; } = "";
		internal override string CommandRead { get; set; } = ":FILE:LIST?";

		public class DirectroyPart
		{
			public bool isdirectory = false;
			public string date;
			public string time;
			public Int64 filesize;
			public string name;
			public string attr;
		}

		public virtual List<DirectroyPart> ReadCommandList(string str = null)
		{
			var dirpart = new List<DirectroyPart>();

			var ret = DevIo?.SendQuery(string.Format(CommandRead));
            ret = GtcDevIo.DevIo.CutStringSpace(ret);
            ret = ret.Replace("\"", "");
            ret = ret.Replace("\r\n", "");
            var part = ret.Split(',');
            foreach (var d in part)
            {
                var parts = d.Split(' ');
                var p = new DirectroyPart();
                if (parts.Length == 5)
                {
                    p.name = parts[0].TrimEnd('\\');

                    p.date = parts[1];
                    p.time = parts[2];
                    try
                    {
                        p.filesize = Int64.Parse(parts[3]);
                        p.attr = parts[4];
                        if (p.attr.IndexOf('d') != -1) p.isdirectory = true;

                        dirpart.Add(p);
                        continue;
                    }
                    catch
                    {
                    }
                }

                var fileStr = d;
                int cnt = 0;

                cnt = fileStr.Length - 4;
                var type = fileStr.Substring(cnt).Trim();
                if (type.IndexOf('d') != -1) p.isdirectory = true;
                if (type.Contains("s")) continue;
                p.attr = type;
                fileStr = fileStr.Substring(0, cnt).Trim();

                if (p.isdirectory == false)
                {
                    cnt = fileStr.LastIndexOf(' ');
                    if (cnt < 0) continue;
                    var size = fileStr.Substring(cnt + 1);
                    if (size.Length != 0)
                    {
                        Int64 res = 0;
                        if (Int64.TryParse(size, out res))
                        {
                            p.filesize = Int64.Parse(size);
                            fileStr = fileStr.Substring(0, cnt);
                        }
                        else
                        {
                            p.filesize = 0;
                        }

                    }
                    else
                    {
                        fileStr = fileStr.Substring(0, cnt);
                    }
                }

                var time = "";
                cnt = fileStr.LastIndexOf(' ');
                if (cnt < 0) continue;
                time = fileStr.Substring(cnt + 1);
                fileStr = fileStr.Substring(0, cnt);

                cnt = fileStr.LastIndexOf(':');
                if (cnt >= 0)
                {
                    //ExFAT
                    cnt = fileStr.LastIndexOf(' ');
                    if (cnt < 0) continue;
                    p.time = fileStr.Substring(cnt);
                    p.date = time;
                    fileStr = fileStr.Substring(0, cnt - 1);
                }
                else
                {
                    p.time = time;
                    cnt = fileStr.LastIndexOf(' ');
                    if (cnt < 0) continue;
                    p.date = fileStr.Substring(cnt + 1);
                    fileStr = fileStr.Substring(0, cnt);
                }
                p.name = fileStr.TrimEnd('\\');

                dirpart.Add(p);
            }

            return dirpart;
        }

        public string ToShiftJis(string unicodeStrings)
		{
			var unicode = Encoding.Unicode;
			var unicodeByte = unicode.GetBytes(unicodeStrings);
			var s_jis = Encoding.GetEncoding("shift_jis");
			var s_jisByte = Encoding.Convert(unicode, s_jis, unicodeByte);
			var s_jisChars = new char[s_jis.GetCharCount(s_jisByte, 0, s_jisByte.Length)];
			s_jis.GetChars(s_jisByte, 0, s_jisByte.Length, s_jisChars, 0);
			return new string(s_jisChars);
		}
	}

	class FileCommandTrans : FileCommand
    {
        public FileCommandTransSour Sour { get; set; } = new FileCommandTransSour();
        public FileCommandTransOutp Outp { get; set; } = new FileCommandTransOutp();
        public FileCommandTransSize Size { get; set; } = new FileCommandTransSize();
        public FileCommandTransOpen Open { get; set; } = new FileCommandTransOpen();
        public FileCommandTransClose Close { get; set; } = new FileCommandTransClose();

        public FileCommandTrans(GtcDevIo.DevIo deviceManager)
        {
            Sour.DevIo = deviceManager;
            Outp.DevIo = deviceManager;
            Size.DevIo = deviceManager;
            Open.DevIo = deviceManager;
            Close.DevIo = deviceManager;
        }
    }

    class FileCommandTransSour : FileCommand
    {
        internal override string CommandSend { get; set; } = ":FILE:TRANS:SOUR \"{0}\"";
        internal override string CommandRead { get; set; } = ":FILE:TRANS:SOUR?";

        public override void SendCommand(string str = null)
        {
            var s = string.Format(CommandSend, str);
            DevIo?.SendCommand(s);
        }
    }

    class FileCommandTransOutp : FileCommand
    {
        internal override string CommandSend { get; set; } = ":FILE:TRANS:OUTP {0}";
        internal override string CommandRead { get; set; } = ":FILE:TRANS:OUTP?";

        public byte[] ReadBinary(string str = null)
        {
            return DevIo?.SendReadData(string.Format(CommandRead), headersize: 8, statussize: 2, checksumsize: 0, timeout: 500);
        }
    }

    class FileCommandTransSize : FileCommand
    {
        internal override string CommandSend { get; set; } = "";
        internal override string CommandRead { get; set; } = ":FILE:TRANS:SIZE?";
    }

    class FileCommandTransOpen : FileCommand
    {
        internal override string CommandSend { get; set; } = "";
        internal override string CommandRead { get; set; } = ":FILE:TRANS:OPEN?";

        public override string ReadCommand(string str = null)
        {
            DevIo?.SendCommand(string.Format(CommandRead));
            var ret = DevIo?.ReadBinary(3, 10000);
            if (ret != null) return "ok";
            return "ng";
        }
    }

    class FileCommandTransClose : FileCommand
    {
        internal override string CommandSend { get; set; } = "";
        internal override string CommandRead { get; set; } = ":FILE:TRANS:CLOSE?";

        public override string ReadCommand(string str = null)
        {
            DevIo?.SendCommand(string.Format(CommandRead));
            var ret = DevIo?.ReadBinary(2, 10000);
            if (ret != null) return "ok";
            return "ng";
        }
    }
}
