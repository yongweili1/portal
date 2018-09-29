import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { Validators, FormControl, FormGroup, FormBuilder, } from '@angular/forms';
import { Router, ActivatedRoute, Params } from '@angular/router';
import { Location } from '@angular/common';

import { EmailTemplateService } from '../../../services/email-template.service';
import { Email, Param } from '../shared/email-template.model';
import { ToastService } from '../../../core/toast.service';

@Component({
  selector: 'mpt-edit-email-template',
  templateUrl: '../shared/email-template-detail.component.html',
  styleUrls: ['../email-template.component.less']
})

export class EditEmailTemplateComponent implements OnInit {

  public isShowCode = true;
  public isNew = false;
  public form: FormGroup;

  public enable = true;

  public emailTemplate: Email = {};

  public parameters: Param[] = [];

  public selectedParameters: Param;

  @ViewChild('textarea') textarea: ElementRef;

  constructor(
    private fb: FormBuilder,
    private router: Router,
    private location: Location,
    private route: ActivatedRoute,
    private toastService: ToastService,
    private emailTemplateService: EmailTemplateService
  ) {
  }

  public ngOnInit() {
    this.buildForm();
    this.route.params
      .switchMap((params: Params) => this.emailTemplateService.getTemplateCode(params['id']))
      .subscribe((res) => {
        this.emailTemplate = res;
        this.buildForm();
      });

    this.emailTemplateService.getParameters('01').subscribe((data) => {
      this.parameters = data;
    });

  }

  private buildForm() {
    if (this.emailTemplate.enable === '0') {
      this.enable = true;
    } else {
      this.enable = false;
    }
    this.form = this.fb.group({
      id: new FormControl(this.emailTemplate.id),
      name: new FormControl(this.emailTemplate.name, Validators.required),
      subject: new FormControl(this.emailTemplate.subject, Validators.required),
      content: new FormControl(this.emailTemplate.content, Validators.required),
      createTime: new FormControl(this.emailTemplate.createTime),
      updateTime: new FormControl(this.emailTemplate.updateTime),
      enable: new FormControl(this.enable),
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

    this.emailTemplateService.updateEmailTemplate(this.form.value).subscribe(
      (result) => {
        this.emailTemplateService.isAddRefreshEvent(this.form.value);
        this.toastService.success('修改成功');
        this.enable = false;
        this.onHide();
      },
      (error) => {
        this.toastService.error('修改失败');
        this.enable = false;
      });
  }

  private onHide(): void {
    this.location.back();
  }

}
