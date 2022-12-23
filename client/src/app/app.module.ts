import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { HttpClientModule } from '@angular/common/http';
import { MatSlideToggleModule } from '@angular/material/slide-toggle';
import { MatTooltipModule } from '@angular/material/tooltip';
import { MatDialogModule } from '@angular/material/dialog';
import { AppRoutingModule } from './app-routing.module';
import { MatSidenavModule } from '@angular/material/sidenav';
import { AppComponent } from '@components/app-component/app.component';
import { SocketIoModule, SocketIoConfig } from 'ngx-socket-io';
import { FormsModule } from '@angular/forms';
import { MatDialogRef } from '@angular/material/dialog';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { MatExpansionModule } from '@angular/material/expansion';
import { MatSelectModule } from '@angular/material/select';
import { MatButtonModule } from '@angular/material/button';
import { MatIconModule } from '@angular/material/icon';
import { MenuComponent } from './components/menu/menu.component';
import { MatMenuModule } from '@angular/material/menu';
import { MatToolbarModule } from '@angular/material/toolbar';
import { MapComponent } from '@components/map/map.component';
import { AngularResizeEventModule } from 'angular-resize-event';
import { CommandComponent } from './components/command/command.component';
import { DroneStateComponent } from './components/drone-state/drone-state.component';
import { MatTableModule } from '@angular/material/table';
import { ConsoleComponent } from './components/console/console.component';
import { MatCardModule } from '@angular/material/card';
import { MatDividerModule } from '@angular/material/divider';
import { ConsoleControlComponent } from './components/console-control/console-control.component';
import { MatPaginatorModule } from '@angular/material/paginator';
import { DataMissionsListComponent } from '@components/data-missions/data-missions-list/data-missions-list.component';
import { MatSortModule } from '@angular/material/sort';
import { MainPageComponent } from './pages/main-page/main-page.component';
import { LogMissionsComponent } from './components/log-missions/log-missions.component';
const config: SocketIoConfig = { url: 'http://localhost:5000/', options: {} };

// Uncomment for test RF10 on multiple devices
// Run : ng serve --host 0.0.0.0 and run on server flask run --host=0.0.0.0
// const config: SocketIoConfig = { url: 'http://10.200.12.53:5000/', options: {} };

@NgModule({
  declarations: [
    AppComponent,
    MenuComponent,
    MapComponent,
    CommandComponent,
    DroneStateComponent,
    ConsoleComponent,
    ConsoleControlComponent,
    DataMissionsListComponent,
    MainPageComponent,
    LogMissionsComponent,
  ],
  imports: [
    BrowserModule,
    HttpClientModule,
    AppRoutingModule,
    MatSlideToggleModule,
    SocketIoModule.forRoot(config),
    FormsModule,
    MatTooltipModule,
    MatDialogModule,
    BrowserAnimationsModule,
    MatSidenavModule,
    MatExpansionModule,
    MatSelectModule,
    MatButtonModule,
    MatIconModule,
    MatMenuModule,
    MatToolbarModule,
    AngularResizeEventModule,
    MatTableModule,
    MatCardModule,
    MatDividerModule,
    MatPaginatorModule,
    MatSortModule,
    MatTableModule,
  ],
  providers: [{ provide: MatDialogRef, useValue: {} }],
  bootstrap: [AppComponent],
})
export class AppModule {}
