import { Injectable } from '@angular/core';
import { Router, ActivatedRoute } from '@angular/router';

@Injectable()
export class RouteService {

  constructor(
    private route: ActivatedRoute,
    private router: Router) { }

  public mergetRouteBy(queryParams, relativeTo) {
    this.router.navigate(['.'],
      {
        queryParams,
        relativeTo,
        queryParamsHandling: 'merge'
      });
  }
}
