import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { Routes, RouterModule } from '@angular/router';
import { SharedModule } from '../../shared/shared.module';



import { TreeModule } from 'primeng/tree';

import { TrainingStatusComponent } from './training-status.component';
import { TrainingStatusSumComponent } from './training-status-sum/training-status-sum.component';
import { TrainingStatusDetailComponent } from './training-status-detail/training-status-detail.component';

const routes: Routes = [
    {
        path: '',
        component: TrainingStatusComponent,
        children: [
            { path: '', component: TrainingStatusSumComponent },
            { path: 'sum', component: TrainingStatusSumComponent },
            { path: 'detail', component: TrainingStatusDetailComponent },
        ]
    },
];


@NgModule({
    imports: [
        CommonModule,
        TreeModule,
        SharedModule,
        RouterModule.forChild(routes),
    ],
    declarations: [TrainingStatusComponent, TrainingStatusSumComponent, TrainingStatusDetailComponent]
})
export class TrainingStatusModule { }
