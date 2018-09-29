import { Component, OnInit, HostListener } from '@angular/core';
import { Validators, FormArray, FormControl, FormGroup, FormBuilder } from '@angular/forms';
import { Location } from '@angular/common';
import { Router, ActivatedRoute, Params } from '@angular/router';

import { TranslateService } from '@ngx-translate/core';
import { VerifyCodeService } from '../../../services/verify-code.service';
import { ForgotPwdInfo } from '../../../models/forgot-password.model';
import { ForgotPasswordService } from '../../../services/forgot-password.service';


@Component({
  selector: 'mpt-reset',
  templateUrl: 'reset.component.html',
  styleUrls: ['../forgot-password.component.less'],
})

export class ResetComponent implements OnInit {
  public form: FormGroup;
  public invalidTimeErrorAlarm = true;
  public rePwdErrorAlarm = false;
  public id;
  private forgotPwdInfo: ForgotPwdInfo = {};


  constructor(
    private fb: FormBuilder,
    private location: Location,
    private route: ActivatedRoute,
    private router: Router,
    private verifyCodeService: VerifyCodeService,
    private forgotPasswordService: ForgotPasswordService
  ) { }

  public ngOnInit(): void {
    this.route.params.subscribe((params: Params) => {
      this.id = params['id'];
      this.getById();
    });
    this.buildForm();
  }

  private getById() {
    this.forgotPwdInfo.id = this.id;
    this.verifyCodeService.checkCode(this.forgotPwdInfo).subscribe((result) => {
      if (result) {
        this.invalidTimeErrorAlarm = true;
      } else {
        this.invalidTimeErrorAlarm = false;
      }
    });
  }

  private buildForm() {
    this.form = this.fb.group({
      password: new FormControl('', [Validators.required, Validators.minLength(6)]),
      confirmPwd: new FormControl('', Validators.required)
    });
  }

  private submitResetForm() {
    if (!this.form.valid) {
      return false;
    }
    if (this.form.value.password !== this.form.value.confirmPwd) {
      this.rePwdErrorAlarm = true;
      return false;
    } else {
      this.rePwdErrorAlarm = false;
    }
    const param = {
      password: this.form.value.password,
      id: this.id
    };
    this.forgotPasswordService.submitResetInfo(param).subscribe((result) => {
      if (result) {
        this.router.navigate(['forgot-password/changed']);
      }
    });
  }
}
