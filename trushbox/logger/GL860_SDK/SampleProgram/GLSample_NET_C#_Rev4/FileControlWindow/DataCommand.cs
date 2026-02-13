using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GtcDevIo;

namespace GtcCommand
{
    class DataCommandGroup
    {
        public DataCommandCapt Capt { get; set; } = new DataCommandCapt();

        public DataCommandGroup(GtcDevIo.DevIo deviceManager)
        {
            Capt.DevIo = deviceManager;
        }
    }

    class DataCommand
    {
        internal GtcDevIo.DevIo DevIo { get; set; }
        internal virtual string CommandSend { get; set; } = "";
        internal virtual string CommandRead { get; set; } = "";

        public virtual void SendCommand(string str = null)
        {
            DevIo?.SendCommand(string.Format(CommandSend, str));
        }

        public virtual string ReadCommand(string str = null)
        {
            return DevIo?.SendQuery(string.Format(CommandRead));
        }
    }

    class DataCommandCapt : DataCommand
    {
        internal override string CommandSend { get; set; } = ":DATA:CAPT DISK,\"{0}\"";
        internal override string CommandRead { get; set; } = ":DATA:CAPT?";
    }
}
