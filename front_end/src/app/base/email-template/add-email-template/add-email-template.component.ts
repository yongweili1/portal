import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { Validators, FormControl, FormGroup, FormBuilder, } from '@angular/forms';
import { Router } from '@angular/router';
import { Location } from '@angular/common';

import { ToastService } from '../../../core/toast.service';
import { Email, Param } from '../shared/email-template.model';
import { EmailTemplateService } from '../../../services/email-template.service';

@Component({
  selector: 'mpt-add-email-template',
  templateUrl: '../shared/email-template-detail.component.html',
  styleUrls: ['../email-template.component.less']
})

export class AddEmailTemplateComponent implements OnInit {

  public isShowCode = false;
  public isNew = true;

  public form: FormGroup;

  public enable = true;

  public emailTemplate: Email = {};

  public parameters: Param[];

  @ViewChild('textarea') textarea: ElementRef;

  constructor(
    private fb: FormBuilder,
    private router: Router,
    private location: Location,
    private toastService: ToastService,
    private emailTemplateService: EmailTemplateService
  ) {
  }

  public ngOnInit() {
    this.buildForm();
    this.emailTemplateService.getParameters('01').subscribe((data) => {
      this.parameters = data;
    });
  }

  private buildForm() {
    this.form = this.fb.group({
      name: new FormControl('', Validators.required),
      subject: new FormControl('', Validators.required),
      content: new FormControl('', Validators.required),
      enable: new FormControl('', Validators.required),
    });
  }

  public onRowSelect(event) {
    const start = this.textarea.nativeElement.selectionStart;
    const value = this.textarea.nativeElement.value;

    if (this.emailTemplate.content === undefined) {
      this.emailTemplate.content = '';
    }
    this.emailTemplate.content = value.substring(0, start) + '{' + event.data.name + '}' + value.substring(start);
  }

  private onSave() {

    if (!this.form.valid) {
      return false;
    }
    if (this.enable) {
      this.form.patchValue({
        enable: '0',
      });
    } else {
      this.form.patchValue({
        enable: '1',
      });
    }

    this.emailTemplateService.addEmailTemplate(this.form.value).subscribe(
      (result) => {
        if (result != null) {
          this.emailTemplateService.isAddRefreshEvent(result);
        }
        this.toastService.success('新增成功');
        this.enable = false;
        this.onHide();
      },
      (error) => {
        this.toastService.error('新增失败');
        this.enable = false;
      }
    );
  }

  private onHide(): void {
    this.location.back();
  }

}
