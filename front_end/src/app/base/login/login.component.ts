import { Component, OnInit } from '@angular/core';
import { Validators, FormControl, FormGroup, FormBuilder } from '@angular/forms';
import { Router } from '@angular/router';
import { AuthService } from '../../core/auth.service';
import { AppConfigService } from '../../app.config';
import { ToastService } from '../../core/toast.service';
import { VerifyCodeService } from '../../services/verify-code.service';

@Component({
  selector: 'mpt-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.less']
})
export class LoginComponent implements OnInit {

  public loginform: FormGroup;

  public msgs = [];

  public codeFlag = false;
  public verifyCodeSrc; // 验证码
  public code; // 输入
  public code_id; // codeid
  public imgSrc = '';
  public countdown = 0;
  public interval;

  constructor(
    private fb: FormBuilder,
    private router: Router,
    private authService: AuthService,
    private toastService: ToastService,
    private appConfig: AppConfigService,
    private verifyCodeService: VerifyCodeService) {
  }

  public ngOnInit() {
    this.loginform = this.fb.group({
      id: new FormControl(''),
      username: new FormControl('', Validators.required),
      password: new FormControl('', [Validators.required, Validators.minLength(6)]),
      code: new FormControl('')
    });
    // this.getCode();
  }

  public login(params): void {
    // if (this.codeFlag) {
    this.authService.login(params.username, params.password)
      .subscribe(() => {
        this.router.navigate(['/home']);
      }, (e) => {
        // TODO 根据status的不同，做不同的判断
        // this.router.navigate(['/renew-password']);
        this.handleError(e);
      });
    // }
  }

  private handleError(error): void {
    switch (error.status) {
      case 401:
        this.msgs.push({
          severity: 'error',
          summary: 'Error Message',
          detail: 'Username or password is wrong.'
        });
        this.toastService.error('Username or password is wrong.');
      // TODO
    }
  }

  private checkCode() {
    this.loginform.value.id = this.code_id;
    this.verifyCodeService.checkCode(this.loginform.value).subscribe((result) => {
      if (result) {
        this.codeFlag = true;
      } else {
        this.codeFlag = false;
      }
    });
  }

  private getCode() {
    this.verifyCodeService.getCode().subscribe((result) => {
      this.code_id = result.id;
      this.imgSrc = `${this.appConfig.apiUrl}/api/verify-code/${this.code_id}`;
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
        this.loginform.value.code = '';
        this.code = '';
        return false;
      } else {
        this.countdown--;
      }
    }, 1000);
  }

}
