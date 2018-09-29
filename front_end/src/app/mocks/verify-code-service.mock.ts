import { Injectable } from '@angular/core';

declare var Mock: any;
const Random = Mock.Random;

@Injectable()
export class VerifyCodeServiceMock { 
  private RandomCode() {
    const codeLength = 4; // 验证码的长度
    let codes = '';
    const random = new Array(
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
      'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
      'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z');
    for (let i = 0; i < codeLength; i++) {// 循环操作
      const index = Math.floor(Math.random() * 61); // 取得随机数的索引（0~35）
      codes += random[index]; // 根据索引取得随机数加到code上
    }
    return codes;
  }

  private RandomCodeSrc() {
    let str = '';
    let img;
    const random = new Array(
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
      'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
      'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z');
    for (let i = 0; i < 4; i++) {
      const n = Math.floor(Math.random() * random.length);
      str = str + random[n];
      img = img + '../images/randomNums/" + n + ".jpg';
    }
    return img;
  }

    constructor() {

      Mock.mock(/\/api\/verify-code/, 'get', (option) => {
        const vrifyCode = this.RandomCode();
        return {
          'id': Random.guid(),
          'code': vrifyCode,
          'invalidTime': 60
        };
      });

      Mock.mock(/\/api\/verify-code\/[A-Za-z0-9-]+/, 'get', (option) => {
        console.log(this.RandomCodeSrc());
        return true;
      });

      Mock.mock(/\/api\/verify-code\/[A-Za-z0-9-]+/, 'post', (option) => {
        return true;
      });

    }
  }
