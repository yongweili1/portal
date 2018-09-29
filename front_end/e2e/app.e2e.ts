import { browser, by, element } from 'protractor';
import 'tslib';

describe('App', () => {

  beforeEach(async () => {
    await browser.get('/#/login');
  });

  it('进入用户登录页面', async () => {
    let subject = await browser.getTitle();
    let result  = '系统管理';
    expect(result).toEqual(subject);
  });

  it('填写用户和密码，点击登录', async () => {
    element(by.name('username')).sendKeys('user');
    element(by.name('password')).sendKeys('password');
    element(by.buttonText('登录')).click();
  });

});
