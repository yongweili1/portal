import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { RouterModule, PreloadAllModules } from '@angular/router';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { HttpClientModule, HttpClient} from '@angular/common/http';
import { TranslateModule, TranslateLoader } from '@ngx-translate/core';
import { TranslateHttpLoader } from '@ngx-translate/http-loader';
import { ROUTES } from './app.routes';
import { AppComponent } from './app.component';
import { AppConfigService } from './app.config';
import { CoreModule } from './core/core.module';
import { SharedModule } from './shared/shared.module';
import { LoginModule } from './base/login/login.module';
import { NoContentComponent } from './base/no-content/no-content.component';
import { HeaderModule } from './components/header/header.module';
import { SideBarModule } from './components/sidebar/sidebar.module';
import { EasyUIModule } from 'ng-easyui/components/easyui/easyui.module';
import { ElModule } from 'element-angular'

import { FullLayoutComponent } from './layouts/full-layout.component';
import { SimpleLayoutComponent } from './layouts/simple-layout.component';

import { MockModule } from './mocks/mock.module';
import { ForgotPasswordModule } from './base/forgot-password/forgot-password.module';
import { RenewPasswordModule } from './base/renew-password/renew-password.module';
import { ContouringModule} from './components/tps/contouring/contouring.module';
import { HomeComponent } from './components/home/home.component'
import {PacsModule} from './components/pacs/pacs.module'

export function HttpLoaderFactory(http: HttpClient) {
  const appConfig = window['appConfig'] || {};
  const localeUrl = appConfig.localeUrl || './assets/i18n/';
  return new TranslateHttpLoader(http, localeUrl, '.json');
}

@NgModule({
  bootstrap: [AppComponent],
  declarations: [
    AppComponent,
    NoContentComponent,
    FullLayoutComponent,
    SimpleLayoutComponent,
    HomeComponent,
  ],
  imports: [
    BrowserModule,
    BrowserAnimationsModule,
    HttpClientModule,
    RouterModule.forRoot(ROUTES, {
      useHash: true,
      preloadingStrategy: PreloadAllModules,
    }),
    TranslateModule.forRoot({
      loader: {
        provide: TranslateLoader,
        useFactory: HttpLoaderFactory,
        deps: [HttpClient]
      }
    }),
    EasyUIModule,
    ElModule.forRoot(),
    MockModule,
    CoreModule,
    SharedModule,
    LoginModule,
    HeaderModule,
    SideBarModule,
    ForgotPasswordModule,
    RenewPasswordModule,
    ContouringModule,
  ],
  providers: [
    AppConfigService
  ]
})
export class AppModule { }
