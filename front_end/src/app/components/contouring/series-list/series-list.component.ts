import { Component, OnInit, Input} from '@angular/core';

@Component({
  selector: 'mpt-series-list',
  templateUrl: './series-list.component.html',
  styleUrls: ['./series-list.component.less']
})
export class SeriesListComponent implements OnInit {

  @Input() public seriesList: Array<string>;
  constructor() { }

  ngOnInit() {
  }

}
