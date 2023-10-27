TSQLServer *db;
TSQLRow *row;
TSQLResult *res;
int getRunList(char* runlist = "run22_phys")
{
  initSQL();
  getRunInfo(runlist);
  clearSQL();
  return 1;
}
void initSQL()
{
  char *server = "mysql://eicsrv1.sdcc.bnl.gov:3306/cnipol";
  //char *server = "mysql://127.0.0.1:2212/cnipol";
  char *user = "cnipol";
  char *psswd = "(n!P0l";  
  db = TSQLServer::Connect(server, user, psswd);
}
void clearSQL()
{
  delete db;
}
void getRunInfo(const char* runlist)
{
  FILE *fp = fopen(runlist, "w");
  char *dbname = "cnipol";
  char *table = "run_info";  
  //char *sql = Form("select run_name,start_time,polarization,measurement_type from %s.%s where run_name < %d and polarization>0.01and polarization < 0.99 and start_time > '2021-12-04 00:00:00' and measurement_type=2", dbname, table, 32934);
  char *sql = Form("select run_name,start_time,polarization,measurement_type from %s.%s where run_name > %d and polarization>0.01and polarization < 0.99 and start_time > '2021-12-04 00:00:00' and measurement_type=2", dbname, table, 32934);
  //char *sql = Form("select run_name,start_time,polarization,measurement_type from %s.%s where run_name > %d and (run_name < 32964 or run_name > 32965) and polarization>0.01and polarization < 0.99 and start_time > '2021-12-04 00:00:00' and measurement_type=2", dbname, table, 32934);
  res = db->Query(sql);

  int nrows = res->GetRowCount();

  int nfields = res->GetFieldCount();
  for (int i = 0; i < nrows; i++) {
    row = res->Next();
    if (strcmp(row->GetField(0), "33074.102")==0) continue;
    fprintf(fp, "%s\n", row->GetField(0));
    //Printf("%s %s %s %s", row->GetField(0), row->GetField(1), row->GetField(2), row->GetField(3));
    delete row;
  }

  delete res;
  fclose(fp);
}
