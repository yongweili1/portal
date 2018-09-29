import { CommonModule } from '@angular/common';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { NgModule } from '@angular/core';

import { TranslateModule } from '@ngx-translate/core';

import {
  SanitizeHtmlPipe, DurationPipe
} from './pipes';
import {
  StylesDirective,
  EchartsDirective,
  HasPermissionDirective, 
  EqualValidator,
} from './directives';
import { ToastComponent } from './toast';
import { CalendarLocales } from './locales/calendar.locales';

import { DateTimeRangePickerModule } from '../base/datetime-range-picker/datetime-range-picker.module';

import { SystemTimeComponent } from './system-time/system-time.component';
import { SystemTimeService } from './system-time/system-time.service';

import { IconButtonComponent } from './icon-button/icon-button.component';

// import { ButtonModule } from './button/button';
// import { MessagesModule } from './messages/messages';
// import { ProgressBarModule } from './progressbar/progressbar';
// import { DomHandler } from './dom/domhandler';
@NgModule({
  imports: [
    CommonModule,
    FormsModule,
    TranslateModule
  ],
  declarations: [
    StylesDirective,
    EchartsDirective,
    HasPermissionDirective,
    EqualValidator,
    SanitizeHtmlPipe,
    DurationPipe,
    ToastComponent,
    SystemTimeComponent,
    IconButtonComponent,

    // ButtonModule,
    // MessagesModule,
    // ProgressBarModule,
    // DomHandler
  ],
  exports: [
    CommonModule,
    FormsModule,
    TranslateModule,
    ReactiveFormsModule,
    ToastComponent,
    SystemTimeComponent,
    StylesDirective,
    EchartsDirective,
    HasPermissionDirective,
    EqualValidator,
    SanitizeHtmlPipe,
    DurationPipe,
    DateTimeRangePickerModule,
    IconButtonComponent,

    // ButtonModule,
    // MessagesModule,
    // ProgressBarModule,
    // DomHandler
  ],
  providers: [
    CalendarLocales,
    SystemTimeService
  ]
})
export class SharedModule {
}
