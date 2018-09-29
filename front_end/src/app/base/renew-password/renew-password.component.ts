import { Component, OnInit } from '@angular/core';
import { FormBuilder, FormGroup, FormControl, Validators } from '@angular/forms';
import { Router } from '@angular/router';
import { Validators as AppValidators } from '../../shared/validators';
import { ToastService } from '../../core/toast.service';

import { RenewPasswordService } from './renew-password.service';

@Component({
  selector: 'mpt-renew-password',
  templateUrl: 'renew-password.component.html',
  styleUrls: ['renew-password.component.less']
})

export class RenewPasswordComponent implements OnInit {

  public form: FormGroup;
  public userName;
  public oldPassword;
  public newPassword;
  public confirmPassword;
  public submitMsgErrorFlag = false;
  constructor(
    private fb: FormBuilder,
    private router: Router,
    private toastService: ToastService,
    private renewPasswordService: RenewPasswordService) {
  }

  public ngOnInit(): void {
    this.buildForm();
  }

  private buildForm() {
    this.form = this.fb.group({
      userName: new FormControl('', Validators.required),
      oldPassword: new FormControl('', [Validators.required, Validators.minLength(6)]),
      newPassword: new FormControl('', [Validators.required, Validators.minLength(6)]),
      confirmPassword: new FormControl('', [Validators.required, Validators.minLength(6)]),
    });
  }

  public onSubmit() {
    if (!this.form.valid) {
      return false;
    }
    this.renewPasswordService.submitUserInfo(this.form.value).subscribe((result) => {
      if (result === true) {
        this.toastService.success('Renew the password successfully.');
        this.router.navigate(['/login']);
      } else {
        this.submitMsgErrorFlag = true;
      }
    });
  }

}
