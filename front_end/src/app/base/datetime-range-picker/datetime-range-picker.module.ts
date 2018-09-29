import { NgModule } from '@angular/core';
import {
    DialogModule,
    ButtonModule,
    CalendarModule,
    SelectButtonModule
} from 'primeng/primeng';
import { FormsModule } from '@angular/forms';

import { TranslateModule } from '@ngx-translate/core';
import { DateTimeRangePickerComponent } from './datetime-range-picker.component';

@NgModule({
    imports: [
        DialogModule,
        ButtonModule,
        CalendarModule,
        TranslateModule,
        SelectButtonModule,
        FormsModule
    ],
    declarations: [
        DateTimeRangePickerComponent
    ],
    exports: [
        DateTimeRangePickerComponent
    ],
    providers: [
    ]
})
export class DateTimeRangePickerModule {
}
