
Timeout_default = 1

class File_trans:

    devio = None

    def __init__(self, dev):
        self.devio = dev

    # Transfer files in the device.
    #  obj: Instance of USB or TCP/IP
    #           The OBJ must be connected.
    #  sour: File path in the device
    #           Int.MEM:  \MEM\FOLDER\FILE.CSV (or GBD)
    #           SD Slot:  \SD\FOLDER\FILE.CSV (or GBD)
    #  dest: PC-side file output path
    def transfar_file(self, sour, dest):
        
        print(self.devio)
        ret = self.open_file(sour)
        if ret == False: return False

        size = self.get_file_size()
        if size == 0: return False

        self.trans_loop(dest, size)
        self.close_file()
        return True

    def open_file(self, sour):
        if self.devio is None:
            print("open file diveio error")
            return False
        r = self.devio.send_read_command(':FILE:TRANS:SOUR "'+ sour + '";SOUR?')
        print(r)
        if r == '':
            print("open file file:trans:sour error")
            return False
        self.devio.send_command(':FILE:TRANS:OPEN?')
        buf = bytes(self.devio.read_binary(3, Timeout_default))    # Read Result
        print(buf)
        if len(buf) != 3:
            print("open file file:trans:open1 error")
            return False
        if buf[2] != 0:
            print("open file file:trans:open2 error")
            return False

        print("File open successfull")
        return True
    
    def close_file(self):
        if self.devio is None:
            print("open file diveio error")
            return False
        self.devio.send_command(':FILE:TRANS:CLOSE?')
        buf = bytes(self.devio.read_binary(2, Timeout_default))    # Read Result
        print(buf)
        if len(buf) != 2:
            print("open file file:trans:close error")
            return False

        print("File close successfull")
        return True

    # Get the file byte size
    #   FILE:TRANS:OPEN? must already have succeeded.
    def get_file_size(self):
        r = self.devio.send_read_command(':FILE:TRANS:SIZE?')
        rr = int(r.split(' ')[1])
        print(f"File size read successfull {rr}")
        return rr

    def trans_loop(self, dest, size):

        f = open(dest, 'wb')

        packet = 1000
        recieved = 0
        st = 1
        ed = packet

        while recieved < size:         
            # Send command
            com = f':FILE:TRANS:OUTP {st},{ed}'
            print(com)
            self.devio.send_command(com)
            self.devio.send_command(':FILE:TRANS:OUTP?')
            msgbuf = bytes(self.devio.read_binary(8))    # Read header
            len = int(msgbuf[2:8].decode()) 
            print(f"Len {len}")
            msgbuf = bytes(self.devio.read_binary(2))    # Read status
            msgbuf2 = bytes(self.devio.read_binary(len, Timeout_default)) # Read data
            print(msgbuf2, type(msgbuf2))
            f.write(msgbuf2)
            recieved += packet
            st += packet
            ed += packet
            if ed > size: ed = size
        
        f.close()