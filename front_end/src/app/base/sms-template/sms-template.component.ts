import { Component, OnInit } from '@angular/core';
import { TranslateService } from '@ngx-translate/core';

import { LazyLoadEvent, ConfirmationService } from 'primeng/primeng';

import { ToastService } from '../../core/toast.service';
import { SmsTemplateService } from '../../services/sms-template.service';
import { Sms } from './shared/sms-template.model';
import { Page } from '../../shared/models/pagination';

@Component({
  selector: 'mpt-sms-template',
  templateUrl: 'sms-template.component.html',
})

export class SmsTemplateComponent implements OnInit {

  public tablePageRows: number[] = [10, 15, 20, 50, 100];

  public smsTemplate: Page<Sms>;

  public name: string;

  constructor(
    private smsTemplateService: SmsTemplateService,
    private translate: TranslateService,
    private confirmationService: ConfirmationService,
    private toastService: ToastService
  ) {
  }

  public ngOnInit(): void {
    this.loadSmsTemplate();

    this.smsTemplateService.onAddRefresEvent((res) => {
      if (res != null) {
        this.loadSmsTemplate();
      }
    });
  }

  public search() {
    this.loadSmsTemplate();
  }

  public loadSmsTemplate(event: LazyLoadEvent = { first: 0, rows: this.tablePageRows[1] }) {
    this.requestSmsTemplate(
      Math.floor(event.first / (event.rows || this.tablePageRows[1])),
      event.rows || this.tablePageRows[1]
    );
  }
  private requestSmsTemplate(page: number = 0, size: number = this.tablePageRows[1]) {
    this.smsTemplateService.getSmsTemplate({
      page,
      size,
      name: this.name,
    }).subscribe((res) => {
      this.smsTemplate = res;
      for (let i = 0; i < this.smsTemplate.content.length; i++) {
        this.smsTemplate.content[i].no = i + 1;
      }
    });
  }

  public delete(sms: Sms) {

    if (!sms) {
      this.toastService.warning('未选择记录');
      return;
    }

    this.confirmationService.confirm({
      header: '删除记录',
      rejectVisible: true,
      acceptVisible: true,
      message: `您确认删除记录吗？`,
      accept: () => {
        this.smsTemplateService.deleteSmsTemplate(sms.id).subscribe((result) => {
          this.toastService.success(`记录删除成功.`);
          this.requestSmsTemplate();
        });
      }
    });

  }
}
