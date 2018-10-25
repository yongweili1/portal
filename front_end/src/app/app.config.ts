import { Injectable } from '@angular/core';

/** 保存全局配置的服务 */
@Injectable()
export class AppConfigService {
  /** Api服务的基础地址 */
  public apiUrl: string;

  /** 推送服务的基础地址 */
  public messagingUrl: string;

  /** 当前使用的语言 */
  public lang: string;

  /** 当前使用的主题 */
  public theme: string;

  public name: string;

  public title: string;

  public company = {};

  constructor() {
    const appConfig = window['appConfig'] || {};
    this.apiUrl = appConfig.apiUrl || (location.protocol + '//' + location.host);
    //this.apiUrl = appConfig.apiUrl;
    this.messagingUrl = appConfig.messagingUrl || (location.protocol + '//' + location.host);
    this.lang = appConfig.locale || 'zh_CN';
    this.theme = appConfig.theme || 'lightness';

    this.name = appConfig.name || 'uAI-Research Cloud Application';
    this.title = appConfig.title || 'uAI-Research Cloud Application';

    this.company = appConfig.company || {};
  }

}
