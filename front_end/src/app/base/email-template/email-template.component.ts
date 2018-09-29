import { Component, OnInit } from '@angular/core';

import { TranslateService } from '@ngx-translate/core';
import { LazyLoadEvent, ConfirmationService } from 'primeng/primeng';

import { ToastService } from '../../core/toast.service';
import { EmailTemplateService } from '../../services/email-template.service';
import { Email } from './shared/email-template.model';
import { Page } from '../../shared/models/pagination';

@Component({
  selector: 'mpt-email-template',
  templateUrl: 'email-template.component.html',
})

export class EmailTemplateComponent implements OnInit {

  public tablePageRows: number[] = [10, 15, 20, 50, 100];

  public emailTemplate: Page<Email>;

  public name: string;

  constructor(
    private emailTemplateService: EmailTemplateService,
    private translate: TranslateService,
    private confirmationService: ConfirmationService,
    private toastService: ToastService
  ) {
  }

  public ngOnInit(): void {
    this.loadEmailTemplate();

    this.emailTemplateService.onAddRefresEvent((res) => {
      if (res != null) {
        this.loadEmailTemplate();
      }
    });
  }

  public search() {
    this.loadEmailTemplate();
  }

  public loadEmailTemplate(event: LazyLoadEvent = { first: 0, rows: this.tablePageRows[1] }) {
    this.requestEmailTemplate(
      Math.floor(event.first / (event.rows || this.tablePageRows[1])),
      event.rows || this.tablePageRows[1]
    );
  }
  private requestEmailTemplate(page: number = 0, size: number = this.tablePageRows[1]) {
    this.emailTemplateService.getEmailTemplate({
      page,
      size,
      name: this.name,
    }).subscribe((res) => {
      this.emailTemplate = res;
      for (let i = 0; i < this.emailTemplate.content.length; i++) {
        this.emailTemplate.content[i].no = i + 1;
      }
    });
  }

  public delete(email: Email) {

    if (!email) {
      this.toastService.warning('未选择记录');
      return;
    }

    this.confirmationService.confirm({
      header: '删除记录',
      rejectVisible: true,
      acceptVisible: true,
      message: `您确认删除记录吗？`,
      accept: () => {
        this.emailTemplateService.deleteEmailTemplate(email.id).subscribe((result) => {
          this.toastService.success(`记录删除成功.`);
          this.requestEmailTemplate();
        });
      }
    });
  }

}
