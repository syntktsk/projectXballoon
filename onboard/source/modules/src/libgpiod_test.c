# include<stdio.h>
# include<unistd.h>
# include<gpiod.h>

int main() {
  const char *appname = "gpiotest";

  struct gpiod_chip *gchip;
  struct gpiod_line *glinein, *glineout;
  int value;

  // GPIOデバイスを開く
  if ((gchip=gpiod_chip_open_lookup("")) == NULL) {
    perror("gpiod_chip_open_lookup");
    return 1;
  }

  // GPIO入力
  // GPIOのピンのハンドラを取得する
  // ここでは20番ピンを使用
  if ((glinein=gpiod_chip_get_line(gchip, 20)) == NULL) {
    perror("gpiod_chip_get_line");
    return 1;
  }

  // GPIOを入力モードに設定する
  if (gpiod_line_request_input(glinein, appname) != 0) {
    perror("gpiod_line_request_input");
  }

  // GPIOの値を取得する
  if ((value=gpiod_line_get_value(glinein)) == -1) {
    perror("gpiod_line_get_value");
  }
  printf("GPIO20=%d\n", value);

  // GPIO出力
  // GPIOのピンのハンドラを取得する
  // ここでは21番ピンを使用
  if ((glineout=gpiod_chip_get_line(gchip, 21)) == NULL) {
    perror("gpiod_chip_get_line");
    return 1;
  }

  // GPIOを出力モードに設定する
  if (gpiod_line_request_output(glineout, appname, 0) != 0) {
    perror("gpiod_line_request_output");
  }

  // GPIOの値を1に設定する
  if ((value=gpiod_line_set_value(glineout, 1)) == -1) {
    perror("gpiod_line_set_value");
  }

  // 出力を維持するためちょっと待つ。
  sleep(5);

  // GPIOの値を0に設定する
  if ((value=gpiod_line_set_value(glineout, 0)) == -1) {
    perror("gpiod_line_set_value");
  }

  // 出力を維持するためちょっと待つ。
  sleep(5);

  // GPIOデバイスを閉じる
  gpiod_chip_close(gchip);
  return 0;
}
