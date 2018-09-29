import { Component, OnInit, ViewChild, ElementRef } from '@angular/core';
import { Validators, FormControl, FormGroup, FormBuilder, } from '@angular/forms';
import { Router, ActivatedRoute, Params } from '@angular/router';
import { Location } from '@angular/common';

import { SmsTemplateService } from '../../../services/sms-template.service';
import { Sms, Param } from '../shared/sms-template.model';
import { ToastService } from '../../../core/toast.service';

@Component({
  selector: 'mpt-edit-sms-template',
  templateUrl: '../shared/sms-template-detail.component.html',
  styleUrls: ['../sms-template.component.less']
})

export class EditSmsTemplateComponent implements OnInit {

  public isShowCode = true;
  public isNew = false;

  public form: FormGroup;

  public enable = true;

  public smsTemplate: Sms = {};

  public parameters: Param[] = [];

  @ViewChild('textarea') private textarea: ElementRef;

  constructor(
    private fb: FormBuilder,
    private router: Router,
    private location: Location,
    private route: ActivatedRoute,
    private toastService: ToastService,
    private smsTemplateService: SmsTemplateService
  ) {
  }

  public ngOnInit() {
    this.buildForm();

    this.route.params
      .switchMap((params: Params) => this.smsTemplateService.getTemplateCode(params['id']))
      .subscribe((res) => {
        this.smsTemplate = res;
        this.buildForm();
      });

    this.smsTemplateService.getParameters('01').subscribe((data) => {
      this.parameters = data;
    });
  }

  private buildForm() {
    if (this.smsTemplate.enable === '0') {
      this.enable = true;
    } else {
      this.enable = false;
    }
    this.form = this.fb.group({
      name: new FormControl(this.smsTemplate.name, Validators.required),
      content: new FormControl(this.smsTemplate.content, Validators.required),
      id: new FormControl(this.smsTemplate.id),
      createTime: new FormControl(this.smsTemplate.createTime),
      updateTime: new FormControl(this.smsTemplate.updateTime),
      enable: new FormControl(this.enable),
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

    this.smsTemplateService.updateSmsTemplate(this.form.value).subscribe(
      (result) => {
        this.smsTemplateService.isAddRefreshEvent(this.form.value);
        this.toastService.success('修改成功');
        this.enable = false;
        this.onHide();
      },
      (error) => {
        this.toastService.error('修改失败');
        this.enable = false;
      }
    );
  }

  private onHide(): void {
    this.location.back();
  }

}
