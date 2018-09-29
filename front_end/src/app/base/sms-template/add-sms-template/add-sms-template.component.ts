import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { Validators, FormControl, FormGroup, FormBuilder, } from '@angular/forms';
import { Router } from '@angular/router';
import { Location } from '@angular/common';

import { ToastService } from '../../../core/toast.service';
import { Sms, Param } from '../shared/sms-template.model';
import { SmsTemplateService } from '../../../services/sms-template.service';

@Component({
  selector: 'mpt-add-sms-template',
  templateUrl: '../shared/sms-template-detail.component.html',
  styleUrls: ['../sms-template.component.less']
})

export class AddSmsTemplateComponent implements OnInit {

  public isShowCode = false;
  public isNew = true;

  public form: FormGroup;

  public enable = true;

  public smsTemplate: Sms = {};

  public parameters: Param[];

  @ViewChild('textarea') textarea: ElementRef;

  constructor(
    private fb: FormBuilder,
    private router: Router,
    private location: Location,
    private toastService: ToastService,
    private smsTemplateService: SmsTemplateService
  ) {
  }

  public ngOnInit() {
    this.buildForm();

    this.smsTemplateService.getParameters('01').subscribe((data) => {
      this.parameters = data;
    });
  }

  private buildForm() {
    this.form = this.fb.group({
      name: new FormControl('', Validators.required),
      content: new FormControl('', Validators.required),
      enable: new FormControl('', Validators.required),
    });
  }

  public onRowSelect(event) {
    const start = this.textarea.nativeElement.selectionStart;
    const value = this.textarea.nativeElement.value;

    if (this.smsTemplate.content === undefined) {
      this.smsTemplate.content = '';
    }
    this.smsTemplate.content = value.substring(0, start) + '{' + event.data.name + '}' + value.substring(start);
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

    this.smsTemplateService.addSmsTemplate(this.form.value).subscribe(
      (result) => {
        if (result != null) {
          this.smsTemplateService.isAddRefreshEvent(result);
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
