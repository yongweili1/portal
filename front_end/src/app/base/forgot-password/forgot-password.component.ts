import { Component, OnInit } from '@angular/core';
import { Validators, FormArray, FormControl, FormGroup, FormBuilder } from '@angular/forms';
import { Location } from '@angular/common';
import { Router } from '@angular/router';
import { AppConfigService } from '../../app.config';

import { VerifyCodeService } from '../../services/verify-code.service';
import { ForgotPasswordService } from '../../services/forgot-password.service';

@Component({
  selector: 'mpt-forgot-password',
  templateUrl: 'forgot-password.component.html',
  styleUrls: ['./forgot-password.component.less'],
})

export class ForgotPasswordComponent implements OnInit {
  public form: FormGroup;
  public errorAlarm = false;
  public verifyCodeSrc; // 验证码
  public code; // 输入
  public codeFlag = false;
  public countdown = 0;
  public interval;
  public token_id; // 主键id
  public imgSrc = '';

  constructor(
    private fb: FormBuilder,
    private location: Location,
    private router: Router,
    private appConfig: AppConfigService,
    private verifyCodeService: VerifyCodeService,
    private forgotPasswordService: ForgotPasswordService
  ) { }

  public ngOnInit(): void {
    this.buildForm();
    this.getCode();
  }

  private buildForm() {
    this.form = this.fb.group({
      id: new FormControl(''),
      userName: new FormControl('', Validators.required),
      email: new FormControl('', [Validators.required,
      Validators.pattern('^[a-z0-9]+([._\\-]*[a-z0-9])*@([a-z0-9]+[-a-z0-9]*[a-z0-9]+.){1,63}[a-z0-9]+$')]),
      code: new FormControl('', Validators.required)
    });
  }

  private checkCode() {
    this.form.value.id = this.token_id;
    this.verifyCodeService.checkCode(this.form.value).subscribe((result) => {
      if (result) {
        this.codeFlag = true;
      } else {
        this.codeFlag = false;
      }
    });
  }

  private submitForm() {
    if (!this.form.valid) {
      return false;
    }
    if (this.codeFlag === true) {
      this.form.value.id = this.token_id;
      this.forgotPasswordService.submitFotgotInfo(this.form.value).subscribe((result) => {
        if (result) {
          this.router.navigate(['/forgot-password/sent-email']);
        } else {
          this.errorAlarm = true;
        }
      });
    }
  } 

  private getCode() {
    this.verifyCodeService.getCode().subscribe((result) => {
      this.token_id = result.id;
      this.imgSrc = `${this.appConfig.apiUrl}/api/verify-code/${this.token_id}`;
      this.verifyCodeSrc = result.code;
      this.countdown = result.invalidTime;
      this.setTime();
    });
  }

  private setTime() {
    clearInterval(this.interval);
    this.countdown = 60;
    this.interval = setInterval(() => {
      if (this.countdown === 0) {
        this.form.value.code = '';
        this.code = '';
        return false;
      } else {
        this.countdown--;
      }
    }, 1000);
  }
}
