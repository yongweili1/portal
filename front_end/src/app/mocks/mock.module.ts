import { NgModule } from '@angular/core';
import { AuthServiceMock } from './auth-service.mock';
import { UserServiceMock } from './user-service.mock';
import { PatientTemplateServiceMock } from './patient-template-service.mock';
import {LoadSeriesServiceMock} from './load-series-service.mock'
import { ConfigServiceMock } from './config-service.mock';
import { LocalesServiceMock } from './locales-service.mock';
import { HomeServiceMock } from './home-service.mock';
import { SmsTemplateServiceMock } from './sms-template-service.mock';
import { EmailTemplateServiceMock } from './email-template-service.mock';

import { ForgotPasswordServiceMock } from './forgot-password-service.mock';
import { VerifyCodeServiceMock } from './verify-code-service.mock';



@NgModule({
  imports: [
  ],
  declarations: [
  ],
  exports: [
  ],
  providers: [
    // /*
    AuthServiceMock,
    UserServiceMock,
    PatientTemplateServiceMock,
    ConfigServiceMock,
    LocalesServiceMock,
    HomeServiceMock,
    SmsTemplateServiceMock,
    EmailTemplateServiceMock,
    ForgotPasswordServiceMock,
    VerifyCodeServiceMock,
    LoadSeriesServiceMock,
    // */
  ]
})
export class MockModule {

  constructor(
    // /*
    private authService: AuthServiceMock,
    private userServiceMock: UserServiceMock,
    private patientTemplateServiceMock: PatientTemplateServiceMock,
    private configServiceMock: ConfigServiceMock,
    private localesServiceMock: LocalesServiceMock,
    private homeServiceMock: HomeServiceMock,
    private smsTemplateServiceMock: SmsTemplateServiceMock,
    private emailTemplateServiceMock: EmailTemplateServiceMock,
    private forgotPasswordServiceMock: ForgotPasswordServiceMock,
    private verifyCodeServiceMock: VerifyCodeServiceMock,
    private loadSeriesServiceMock:LoadSeriesServiceMock
    // */
  ) { }

}
