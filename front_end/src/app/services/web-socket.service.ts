import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { AppConfigService } from '../app.config';

@Injectable({
    providedIn: 'root'
})
export class WebSocketService {
    ws: WebSocket;
    constructor(private appConfig: AppConfigService) { }
    createObservableSocket(): Observable<any> {
        this.ws = new WebSocket(`${this.appConfig.wsApiUrl}/patientinformations/websocket`);
        return Observable.create(
            observer => {
                this.ws.onopen = () => observer.next('Websocket open');
                this.ws.onmessage = (event) => observer.next(event.data);
                this.ws.onerror = (event) => observer.error(event);
                this.ws.onclose = () => observer.complete();
            })
    }
    // 向服务器端发送消息
    sendMessage(message: any) {
        this.ws.send(JSON.stringify(message));
    }
    closeWebSocket(){
        this.ws.close();
    }
}
