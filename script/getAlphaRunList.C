TSQLServer *db;
TSQLRow *row;
TSQLResult *res;
int getAlphaRunList(char* runlist = "run22_alpha")
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
  char *sql = Form("select run_name from %s.%s where start_time > '2021-12-04 00:00:00' and run_name like '%%alpha%%' and measurement_type=1 and nevents_processed > 0", dbname, table);
  res = db->Query(sql);

  int nrows = res->GetRowCount();

  int nfields = res->GetFieldCount();
  for (int i = 0; i < nrows; i++) {
    row = res->Next();
    fprintf(fp, "%s\n", row->GetField(0));
    //Printf("%s %s %s %s", row->GetField(0), row->GetField(1), row->GetField(2), row->GetField(3));
    delete row;
  }

  delete res;
  fclose(fp);
}
