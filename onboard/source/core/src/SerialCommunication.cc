#include "SerialCommunication.hh"
#include <thread>
#include <chrono>

namespace balloon {

SerialCommunication::SerialCommunication()
  :baudrate_(B9600), openMode_(O_RDWR | O_NONBLOCK)
{
  serialPath_ = "/dev/null";
  tio_ = std::make_unique<termios>();
}

SerialCommunication::SerialCommunication(const std::string& serial_path, speed_t baudrate, mode_t open_mode)
{
  tio_ = std::make_unique<termios>();
  serialPath_ = serial_path;
  baudrate_ = baudrate;
  openMode_ = open_mode;
}

SerialCommunication::~SerialCommunication()
{
  close(fd_);
}
int SerialCommunication::initialize()
{
  // 1. まずポートを開く (O_NONBLOCK は後で fcntl で制御するのでここでは外してもOK)
  fd_ = open(serialPath_.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
  if (fd_ < 0) {
    std::cout << "Open Error: " << serialPath_ << std::endl;
    return -1;
  }

  // 2. 現在の設定を OS から読み出す
  if (tcgetattr(fd_, tio_.get()) != 0) {
    std::cerr << "tcgetattr failed" << std::endl;
    return -1;
  }

  // 3. 速度を設定する (baudrate_ が B1200 などの定数であることを前提)
  cfsetospeed(tio_.get(), baudrate_);
  cfsetispeed(tio_.get(), baudrate_);

  // 4. Rawモード（余計な加工をしない）に設定
  cfmakeraw(tio_.get());

  // 5. 各種フラグを明示的にセット
  tio_->c_cflag |= (CLOCAL | CREAD); // 制御線を無視、受信有効
  tio_->c_cflag &= ~PARENB;          // パリティなし
  tio_->c_cflag &= ~CSTOPB;          // ストップビット1
  tio_->c_cflag &= ~CSIZE;
  tio_->c_cflag |= CS8;              // データ長8bit

  // 6. 設定を反映させる (TCSANOW = 今すぐ)
  if (tcsetattr(fd_, TCSANOW, tio_.get()) != 0) {
    std::cerr << "tcsetattr failed" << std::endl;
    return -1;
  }

  // 7. 非ブロックモードにするなら最後に行う
  if (fcntl(fd_, F_SETFL, openMode_) < 0) {
    std::cout << "fcntl failed" << std::endl;
    return -1;
  }

  std::cout << "BAUDRATE was set to " << baudrate_ << " (B1200 is 9)" << std::endl;
  std::cout << "Open Serial port: " << serialPath_ << " (FD: " << fd_ << ")" << std::endl;
  
  return 0;
}
int SerialCommunication::sreadSingle(uint8_t& buf)
{
  const int length = 1;
  const int status = read(fd_, &buf, length);
  return status;
}

int SerialCommunication::sread(std::vector<uint8_t>& buf, int length)
{
  const int status = read(fd_, &buf[0], length);
  return status;
}

int SerialCommunication::swrite(const std::vector<uint8_t>& buf)
{
  const int length = buf.size();
  int rem = length;
  std::vector<int> counts;
  const int max_send = 1000;
  const int sleep_ms = 250;
  while (rem>0) {
    const int v = std::min(rem, max_send);
    counts.push_back(v);
    rem -= v;
  }

  int num_sent = 0;
  int index = 0;
  for (int c: counts) {
    const int status = write(fd_, &buf[index], c);
    index += c;
    if (status<0) {
      return status;
    }
    num_sent += status;
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
  }
  
  return num_sent;
}

} /* namespace balloon */


