import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { MainPageComponent } from './pages/main-page/main-page.component';
import { DataMissionsListComponent } from '@components/data-missions/data-missions-list/data-missions-list.component';

const routes: Routes = [
  {
    path:'dataMissions',
    component: DataMissionsListComponent
  },
  {
    path: '',
    component: MainPageComponent
  }

];

@NgModule({
    imports: [RouterModule.forRoot(routes, { useHash: true })],
    exports: [RouterModule],
})
export class AppRoutingModule {}
