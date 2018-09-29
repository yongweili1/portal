import { Component, OnInit } from '@angular/core';
import { styles } from './toast.component.styles';
import { Toast, ToastService } from '../../core/toast.service';

@Component({
  selector: 'mpt-toast',
  templateUrl: 'toast.component.html',
})
export class ToastComponent implements OnInit {

  public styles: any = styles;

  public toasts: Toast[] = [];

  constructor(private toastService: ToastService) {
  }

  public ngOnInit(): void {
    this.toastService.events
      .debounceTime(500)
      .map((toast) => this.applyStyle(toast))
      .do((toast) => this.addToast(toast))
      .delay(4800)
      .do((toast) => this.fadeOut(toast))
      .delay(200)
      .do((toast) => this.removeToast(toast))
      .subscribe();
  }

  private applyStyle(toast): Toast {
    return {
      ...toast,
      styles: [styles.toastBase, styles[toast.level]],
    };
  }

  private addToast(toast) {
    this.toasts.push(toast);
  }

  private removeToast(toast) {
    this.toasts = this.toasts.filter(t => t !== toast);
  }

  private fadeOut(toast: Toast) {
    toast.styles = [...toast.styles, styles.fadeOut];
  }

}
