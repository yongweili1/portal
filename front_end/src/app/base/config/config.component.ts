import { Component, OnInit } from '@angular/core';
import { SelectItem, TreeNode } from 'primeng/primeng';

import { ConfItem } from './config.model';
import { ConfigService } from './config.service';

@Component({
    selector: 'mpt-config',
    templateUrl: './config.component.html'
})

export class ConfigComponent implements OnInit {

    public items: ConfItem[];

    constructor(private configService: ConfigService) { }

    public ngOnInit() {
        this.configService.all().subscribe((data) => {
            this.items = data;
        }, (e) => this.handleError(e));
    }

    private handleError(error) {
        // TODO
    }

    private save() {
        // TODO
    }

    private filter(deptName: String) {
        // TODO
    }

}
