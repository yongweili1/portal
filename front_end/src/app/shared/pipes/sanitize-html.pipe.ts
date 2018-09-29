import { Pipe, PipeTransform } from '@angular/core';
import { DomSanitizer, SafeHtml } from '@angular/platform-browser';

@Pipe({
  name: 'mySanitizeHtml'
})

export class SanitizeHtmlPipe implements PipeTransform {
  public constructor(private _sanitizer: DomSanitizer) { }
  public transform(html: string): SafeHtml {
    return this._sanitizer.bypassSecurityTrustHtml(html);
  }
}
