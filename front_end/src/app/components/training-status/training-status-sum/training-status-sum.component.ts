import { Component, OnInit } from '@angular/core';
import { Router } from '@angular/router';
import { TreeNode } from 'primeng/api';
import { TaskService } from '../../../services/task.service'

@Component({
    selector: 'mpt-training-status-sum',
    templateUrl: './training-status-sum.component.html',
    styleUrls: ['./training-status-sum.component.less']
})
export class TrainingStatusSumComponent implements OnInit {

    statusTree: TreeNode[];
    constructor(
        private router: Router,
        private TaskService: TaskService
        ) { }

    ngOnInit() {
        this.TaskService.getHardWareStatus().subscribe(result => {
            //let statusData = JSON.parse(result);
            console.log(result.data)
            this.statusTree = [{
                label: "cluster_1",
                expanded:true,
                children: result.data
            }];
        }

        )
    }

    goTest() {
        this.router.navigate(['/base/training-status/detail']);
    }

}
