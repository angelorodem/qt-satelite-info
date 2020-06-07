#include "janela.h"
#include "ui_janela.h"


Janela::Janela(QWidget *parent) : QMainWindow(parent), ui(new Ui::Janela){
    ui->setupUi(this);

    scene = new QGraphicsScene(this);
    ui->satview->setScene(scene);


    brush_branco = new QBrush(Qt::white);
    brush_preto = new QBrush(Qt::black);
    brush_amarelo = new QBrush(Qt::yellow);
    brush_azul = new QBrush(Qt::blue);
    pen_preto = new QPen(Qt::black);

    pen_preto->setWidth(1);

    scene->addEllipse(100,100,200,200,*pen_preto,*brush_branco);
    scene->addEllipse(150,150,100,100,*pen_preto,*brush_branco);


    scene->addLine(200,300,200,100,*pen_preto);
    scene->addLine(300,200,100,200,*pen_preto);
    ui->satview->scale(-1,1);


    com = new QSerialPort(this);
    serialBuffer = "";



    refresh_com();


}

void Janela::refresh_com(){
    for (int var = 0; var < ui->com->count()+1; ++var) {
        ui->com->removeItem(ui->com->currentIndex());
    }

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        ui->com->addItem( serialPortInfo.portName() + " | " + serialPortInfo.description() );
    }


}

void Janela::readSerial()
{
    QRegExp sep("(\n|\r)");
    QStringList buffer_split = serialBuffer.split(sep,QString::SkipEmptyParts); //  split the serialBuffer string, parsing with ',' as the separator


    if(buffer_split.length() < 5){
        serialData = com->readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }else{
        serialBuffer.clear();
        decodifica(buffer_split);
        qDebug() <<"Pacotes inválidos:" << pacotes_invalidos;
        qDebug() <<"Bytes em espera:" << com->bytesAvailable();

    }

}

void Janela::desenha_sat(float raio, float azimuth, bool usado, int num){

    float x =195 + (raio - 100) * sin((azimuth*M_PI)/180);
    float y =195 + (raio - 100) * cos((azimuth*M_PI)/180);

    if(usado)
        sats_grap.push_back(scene->addEllipse(x,y,10,10,*pen_preto,*brush_amarelo));
    else
        sats_grap.push_back(scene->addEllipse(x,y,10,10,*pen_preto,*brush_preto));
    sat_ref.insert(num,(int)(sats_grap.size()-1));

}

void Janela::limpa_sat(){
    sat_ref.clear();

    for (unsigned var = 0; var < sats_grap.size(); ++var) {

        delete sats_grap.at(var);


    }
    if(!sats_grap.empty()){
        scene->clear();
        sats_grap.clear();
        scene->addEllipse(100,100,200,200,*pen_preto,*brush_branco);
        scene->addEllipse(150,150,100,100,*pen_preto,*brush_branco);
        scene->addLine(200,300,200,100,*pen_preto);
        scene->addLine(300,200,100,200,*pen_preto);

    }

}

Janela::~Janela()
{
    delete ui;
}

bool Janela::checksum(QString pacote){
    short start = 0;
    while (pacote.at(start) != '*') {
        start++;
        if(start >= pacote.size()){
            pacotes_invalidos++;
            return false;
        }
    }
    QString to_chk = pacote.mid(1,start-1);
    pacote.remove(0,start+1);
    int cksm = 0;

    foreach (QChar letra, to_chk) {
        cksm = cksm ^ letra.toLatin1();
    }

    if(cksm == pacote.toInt(nullptr,16)){
        return true;
    }
    pacotes_invalidos++;
    return false;


}

void Janela::GPGGA(QString pacote){
    QRegExp sep("(\\,|\\*)");
    QStringList pkt_data = pacote.split(sep);
    gpgga_data pacote_data;


    pacote_data.A_hora = pkt_data[1];
    pacote_data.B_latitude = pkt_data[2];
    pacote_data.C_latitude_dir = pkt_data[3].toStdString()[0];
    pacote_data.D_longitude = pkt_data[4];
    pacote_data.E_longitude_dir = pkt_data[5].toStdString()[0];
    pacote_data.F_qualidade_fixa = pkt_data[6].toInt();
    pacote_data.G_quantidade_satelites = pkt_data[7].toInt();
    pacote_data.H_diluicao_da_precisao_horizontal = pkt_data[8].toFloat();
    pacote_data.I_altitude = pkt_data[9].toFloat();
    pacote_data.J_unidade_medida_altura = pkt_data[10].toStdString()[0];
    pacote_data.K_altura_geoide_acima_elipsoide = pkt_data[11].toFloat();
    pacote_data.L_unidade_medida_geoide = pkt_data[12].toStdString()[0];
    pacote_data.M_idade_ultima_atualizacao_dgps = pkt_data[13].toFloat();
    pacote_data.N_dgps_id = pkt_data[14].toInt();


    QString hora;
    hora = pacote_data.A_hora.mid(0,6);
    int inserts = 0;
    for (int var = 1; var < hora.size()-inserts; ++var) {
        if(var % 2 == 0){
            hora.insert(var+inserts,':');
            inserts++;
        }
    }
    ui->hora->setText(hora);
    ui->precisao->setText(QString::number(pacote_data.H_diluicao_da_precisao_horizontal));
    ui->altitude->setText(QString::number(pacote_data.I_altitude));


    int inteiro;
    double real;
    QString val;
    while (pacote_data.B_latitude[0] == '0') {
        pacote_data.B_latitude.remove(0,1);
    }
    val = pacote_data.B_latitude.mid(0,2);
    pacote_data.B_latitude = pacote_data.B_latitude.remove(0,2);
    inteiro = val.toInt();
    val = pacote_data.B_latitude;
    real = val.toDouble();


    if(pacote_data.C_latitude_dir == 'S' || pacote_data.C_latitude_dir == 's')
        ui->latitude->setText(QString::number(((real/60)+inteiro)*-1,'g',9));
    else
        ui->latitude->setText(QString::number(((real/60)+inteiro),'g',9));


    while (pacote_data.D_longitude[0] == '0') {
        pacote_data.D_longitude.remove(0,1);
    }

    val = pacote_data.D_longitude.mid(0,2);
    pacote_data.D_longitude = pacote_data.D_longitude.remove(0,2);
    inteiro = val.toInt();
    val = pacote_data.D_longitude;
    real = val.toDouble();


    if(pacote_data.E_longitude_dir == 'W' || pacote_data.E_longitude_dir == 'w')
        ui->longitude->setText(QString::number(((real/60)+inteiro)*-1,'g',9));
    else
        ui->longitude->setText(QString::number(((real/60)+inteiro),'g',9));




}

void Janela::GPGSA(QString pacote){
    QRegExp sep("(\\,|\\*)");
    QStringList pkt_data = pacote.split(sep);
    gpgsa_data pacote_data;
    pacote_data.CN_ids_satelites = new int[12];
    for (int var = 0; var < 12; ++var) {
        pacote_data.CN_ids_satelites[var] = -1;
    }
    pacote_data.A_modo = pkt_data[1].toStdString()[0];
    pacote_data.B_modo_dimensional = pkt_data[2].toStdString()[0];
    pacote_data.CN_ids_satelites[0] = pkt_data[3].toInt();
    pacote_data.CN_ids_satelites[1] = pkt_data[4].toInt();
    pacote_data.CN_ids_satelites[2] = pkt_data[5].toInt();
    pacote_data.CN_ids_satelites[3] = pkt_data[6].toInt();
    pacote_data.CN_ids_satelites[4] = pkt_data[7].toInt();
    pacote_data.CN_ids_satelites[5] = pkt_data[8].toInt();
    pacote_data.CN_ids_satelites[6] = pkt_data[9].toInt();
    pacote_data.CN_ids_satelites[7] = pkt_data[10].toInt();
    pacote_data.CN_ids_satelites[8] = pkt_data[11].toInt();
    pacote_data.CN_ids_satelites[9] = pkt_data[12].toInt();
    pacote_data.CN_ids_satelites[10] = pkt_data[13].toInt();
    pacote_data.CN_ids_satelites[11] = pkt_data[14].toInt();
    pacote_data.O_PDOP = pkt_data[15].toFloat();
    pacote_data.P_HDOP = pkt_data[16].toFloat();
    pacote_data.Q_VDOP = pkt_data[17].toFloat();
    satelites_a.clear();
    int cc = 0;
    for (int var = 0; var < 12; ++var) {
        if(pacote_data.CN_ids_satelites[var] != 0 && pacote_data.CN_ids_satelites[var] != -1){
            satelites_a.push_back(pacote_data.CN_ids_satelites[var]);
            cc++;
        }
    }

    ui->satelites_ativos->setText(QString::number(cc));


    delete[] pacote_data.CN_ids_satelites;

}

void Janela::GPGSV(QString pacote){
    QRegExp sep("(\\,|\\*)");
    QStringList pkt_data = pacote.split(sep);
    gpgsv_data pacote_data;

    pacote_data.A_total_mensagens = pkt_data.at(1).toInt();
    pacote_data.B_numero_desta_mensagem  = pkt_data.at(2).toInt();
    pacote_data.C_satelites_visiveis  = pkt_data.at(3).toInt();
    pacote_data.DS_sats[0].A_sv_prn = pkt_data.at(4).toInt();
    pacote_data.DS_sats[0].B_elevacao_graus = pkt_data.at(5).toInt();
    pacote_data.DS_sats[0].C_azimuth = pkt_data.at(6).toInt();
    pacote_data.DS_sats[0].D_snr = pkt_data.at(7).toInt();
    pacote_data.DS_sats[1].A_sv_prn = pkt_data.at(8).toInt();
    pacote_data.DS_sats[1].B_elevacao_graus = pkt_data.at(9).toInt();
    pacote_data.DS_sats[1].C_azimuth = pkt_data.at(10).toInt();
    pacote_data.DS_sats[1].D_snr = pkt_data.at(11).toInt();
    pacote_data.DS_sats[2].A_sv_prn = pkt_data.at(12).toInt();
    pacote_data.DS_sats[2].B_elevacao_graus = pkt_data.at(13).toInt();
    pacote_data.DS_sats[2].C_azimuth = pkt_data.at(14).toInt();
    pacote_data.DS_sats[2].D_snr = pkt_data.at(15).toInt();
    pacote_data.DS_sats[3].A_sv_prn = pkt_data.at(16).toInt();
    pacote_data.DS_sats[3].B_elevacao_graus = pkt_data.at(17).toInt();
    pacote_data.DS_sats[3].C_azimuth = pkt_data.at(18).toInt();
    pacote_data.DS_sats[3].D_snr = pkt_data.at(19).toInt();

    ui->satelites_visiveis->setText(QString::number(pacote_data.C_satelites_visiveis));

    if(satelites_v.size() != pacote_data.A_total_mensagens){
        if(satelites_v.size() < pacote_data.A_total_mensagens){
            int va = pacote_data.A_total_mensagens - satelites_v.size();
            for(;0 < va;va--){
                satelites_v.push_back(new satelite[4]);
            }
        }

        if(satelites_v.size() > pacote_data.A_total_mensagens){
            int va = satelites_v.size() - pacote_data.A_total_mensagens;
            for(;0 < va;va--){
                delete[] satelites_v.back();
                satelites_v.pop_back();
                satelites_v.shrink_to_fit();
            }
        }}


    for (int var = 0; var < 4; ++var) {
        if(pacote_data.DS_sats[var].C_azimuth != -1 || pacote_data.DS_sats[var].A_sv_prn != -1){
            satelites_v[pacote_data.B_numero_desta_mensagem - 1][var].A_sv_prn = pacote_data.DS_sats[var].A_sv_prn;
            satelites_v[pacote_data.B_numero_desta_mensagem - 1][var].B_elevacao_graus = pacote_data.DS_sats[var].B_elevacao_graus;
            satelites_v[pacote_data.B_numero_desta_mensagem - 1][var].C_azimuth = pacote_data.DS_sats[var].C_azimuth;
            satelites_v[pacote_data.B_numero_desta_mensagem - 1][var].D_snr = pacote_data.DS_sats[var].D_snr;
        }
    }

    if(pacote_data.A_total_mensagens == pacote_data.B_numero_desta_mensagem){
        for(;;){
            if(sat_botoes.size() == 0 || layouts.size() == 0){
                break;
            }
            delete sat_botoes.back();
            sat_botoes.pop_back();
            sat_botoes.shrink_to_fit();
            if(layouts.back()->count() == 0){
                delete layouts.back();
                layouts.pop_back();
                layouts.shrink_to_fit();
            }
        }


        limpa_sat();
        for (unsigned var = 0; var < satelites_v.size(); ++var) {
            for (unsigned vara = 0; vara < 4; ++vara) {
                if (satelites_v[var][vara].A_sv_prn == -1 || (
                            satelites_v[var][vara].B_elevacao_graus == -1 &&
                            satelites_v[var][vara].C_azimuth == -1 &&
                            satelites_v[var][vara].D_snr == -1)){
                    continue;
                }
                if (satelites_v[var][vara].A_sv_prn == 0 || (
                            satelites_v[var][vara].B_elevacao_graus == 0 &&
                            satelites_v[var][vara].C_azimuth == 0 &&
                            satelites_v[var][vara].D_snr == 0)){
                    continue;
                }


                if (layouts.size() == 0) {
                    layouts.push_back(new QVBoxLayout);
                    ui->satelite_layout->addLayout(layouts.back());
                }
                bool ex = false;
                if(layouts[ui->satelite_layout->count()-1]->count() < 3){
                    sat_botoes.push_back(new QPushButton(QString::number(satelites_v[var][vara].A_sv_prn)));
                    connect(sat_botoes.back(),SIGNAL(clicked(bool)),this,SLOT(sat_but_clicked()));
                    for (unsigned vear = 0; vear < satelites_a.size(); ++vear) {
                        if(satelites_v[var][vara].A_sv_prn == satelites_a[vear]){
                            QPalette pal = sat_botoes.back()->palette();
                            pal.setColor(QPalette::Button, QColor(Qt::blue));
                            sat_botoes.back()->setAutoFillBackground(true);
                            sat_botoes.back()->setPalette(pal);
                            sat_botoes.back()->update();
                            ex=true;
                        }
                    }
                    if(ex)
                        desenha_sat(satelites_v[var][vara].B_elevacao_graus * 1.11,satelites_v[var][vara].C_azimuth,true,satelites_v[var][vara].A_sv_prn);
                    else
                        desenha_sat(satelites_v[var][vara].B_elevacao_graus * 1.11,satelites_v[var][vara].C_azimuth,false,satelites_v[var][vara].A_sv_prn);
                    layouts[ui->satelite_layout->count()-1]->addWidget(sat_botoes.back());
                }else{
                    if(layouts.size() > 8){
                        break;
                    }
                    bool ex = false;
                    layouts.push_back(new QVBoxLayout);
                    ui->satelite_layout->addLayout(layouts.back());
                    sat_botoes.push_back(new QPushButton(QString::number(satelites_v[var][vara].A_sv_prn)));
                    connect(sat_botoes.back(),SIGNAL(clicked(bool)),this,SLOT(sat_but_clicked()));
                    for (unsigned vear = 0; vear < satelites_a.size(); ++vear) {
                        if(satelites_v[var][vara].A_sv_prn == satelites_a[vear]){
                            ex=true;
                            QPalette pal = sat_botoes.back()->palette();
                            pal.setColor(QPalette::Button, QColor(Qt::blue));
                            sat_botoes.back()->setAutoFillBackground(true);
                            sat_botoes.back()->setPalette(pal);
                            sat_botoes.back()->update();

                        }
                    }
                    if(ex)
                        desenha_sat(satelites_v[var][vara].B_elevacao_graus * 1.11,satelites_v[var][vara].C_azimuth,true,satelites_v[var][vara].A_sv_prn);
                    else
                        desenha_sat(satelites_v[var][vara].B_elevacao_graus * 1.11,satelites_v[var][vara].C_azimuth,false,satelites_v[var][vara].A_sv_prn);
                    layouts[ui->satelite_layout->count()-1]->addWidget(sat_botoes.back());
                }
            }
        }
    }
}

void Janela::GPRMC(QString pacote){
    QRegExp sep("(\\,|\\*)");
    QStringList pkt_data = pacote.split(sep);
    gprmc_data pacote_data;

    pacote_data.A_hora = (int)pkt_data[1].toFloat();
    pacote_data.B_warning = pkt_data[2].toStdString()[0];
    pacote_data.C_latitude = pkt_data[3].toFloat();
    pacote_data.D_latitude_coord = pkt_data[4].toStdString()[0];
    pacote_data.E_longitude = pkt_data[5].toFloat();
    pacote_data.F_longitude_coord = pkt_data[6].toStdString()[0];
    pacote_data.G_velocidade_nos = pkt_data[7].toFloat();
    pacote_data.H_CMG = pkt_data[8].toFloat();
    pacote_data.I_data = pkt_data[9].toInt();
    pacote_data.J_variacao_magnetica = pkt_data[10].toFloat();
    pacote_data.K_variacao_magnetica_coord = pkt_data[11].toStdString()[0];


    ui->velocidade->setText(QString::number(pacote_data.G_velocidade_nos * 1.852));

}

void Janela::sat_but_clicked(){
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    static QBrush* last;
    static int last_sat=-1;
    QString aaa(button->text());
    int inx=0;

    for(auto e : sat_ref.keys())
    {
        if(e == aaa.toInt())
            inx = sat_ref.value(e);
    }


    if(last_sat != -1){
        sats_grap[last_sat]->setBrush(*last);
    }


    if(sats_grap[inx]->brush() == *brush_preto){
        last = brush_preto;
    }else if(sats_grap[inx]->brush() == *brush_amarelo){
        last=brush_amarelo;
    }
    last_sat = inx;
    sats_grap[inx]->setBrush(*brush_azul);


    for (unsigned var = 0; var < satelites_v.size(); ++var) {
        for (int vara = 0; vara < 4; ++vara) {
            if (QString::number(satelites_v[var][vara].A_sv_prn) == button->text() ) {
                ui->svprn->setText(QString::number( satelites_v[var][vara].A_sv_prn));
                ui->elevacao->setText(QString::number(satelites_v[var][vara].B_elevacao_graus));
                ui->azimuth->setText(QString::number(satelites_v[var][vara].C_azimuth));
                ui->sinal->setText(QString::number(satelites_v[var][vara].D_snr));

            }
        }
    }
}

void Janela::decodifica(QStringList pacotes){
    QString tipo;
    int starting_point;

    foreach (QString pacote, pacotes) {
        starting_point=0;
        if(pacote == "")
            continue;
        while (pacote.at(starting_point) != '$') {
            starting_point++;
            if (starting_point >= pacote.size()) {
                goto bail;
            }
        }

        pacote.remove(0,starting_point);

        tipo = pacote.mid(1,5);

        if(tipo == "GPGSV"){
            if(checksum(pacote)){
                GPGSV(pacote);
            }
        }else if(tipo == "GPGGA"){
            if(checksum(pacote)){
                GPGGA(pacote);
            }
        }else if(tipo == "GPGSA"){
            if(checksum(pacote)){
                GPGSA(pacote);
            }
        }else if(tipo == "GPRMC"){
            if(checksum(pacote)){
                GPRMC(pacote);
            }

        }else {
            pacotes_invalidos++;
        }

        continue;
bail:
        pacotes_invalidos++;
        continue;
    }


}

void Janela::on_abrir_bot_clicked()
{
    com->close();
    com->setPortName(ui->com->currentText().split(" ")[0]);
    com->open(QSerialPort::ReadWrite);
    com->setBaudRate(QSerialPort::Baud9600);
    com->setDataBits(QSerialPort::Data8);
    com->setFlowControl(QSerialPort::NoFlowControl);
    com->setParity(QSerialPort::NoParity);
    com->setStopBits(QSerialPort::OneStop);
    QObject::connect(com, SIGNAL(readyRead()), this, SLOT(readSerial()));

}

void Janela::on_mapa_bot_clicked()
{

    if(ui->longitude->text() != "" && ui->latitude->text() != ""){
        QString link = "https://www.google.com.br/maps/place/"+ ui->latitude->text()+" "+ui->longitude->text();
        QDesktopServices::openUrl(QUrl(link, QUrl::TolerantMode));
    }
}

void Janela::on_fechar_bot_clicked()
{
    com->close();
}

void Janela::on_refresh_bot_clicked(){
    refresh_com();
}

void Janela::on_actionIngles_triggered()
{
    ui->azimute_l->setText("Azimuth");
    ui->elevacao_l->setText("Elevation");
    ui->fechar_bot->setText("Close COM");
    ui->hora_l->setText("Hour (UTC)");
    ui->mapa_bot->setText("See on Google maps");
    ui->precisao_l->setText("Horizontal dilution of precision");
    ui->refresh_bot->setText("Refresh");
    ui->satativos_l->setText("Active Satellites");
    ui->satelite_gp->setTitle("Satellites");
    ui->sats_gp->setTitle("Sattelites (Visible and Active)");
    ui->satvisiveis_l->setText("Visible Satellites");
    ui->sinal_l->setText("Signal (db)");
    ui->velocidade_l->setText("Speed (Km/h)");
    ui->abrir_bot->setText("Open COM");
    ui->menuIdioma->setTitle("Language");
    ui->actionIngles->setText("English");
    ui->actionPortugu_s->setText("Portuguese");

}

void Janela::on_actionPortugu_s_triggered()
{
    ui->azimute_l->setText("Azimute");
    ui->elevacao_l->setText("Elevação");
    ui->fechar_bot->setText("Fechar COM");
    ui->hora_l->setText("Hora (UTC)");
    ui->mapa_bot->setText("Veja no Google maps");
    ui->precisao_l->setText("Diluição da precisão horizontal:");
    ui->refresh_bot->setText("Atualizar");
    ui->satativos_l->setText("Satelites ativos");
    ui->satelite_gp->setTitle("Satelites");
    ui->sats_gp->setTitle("Satelites (Visiveis e Ativos)");
    ui->satvisiveis_l->setText("Satelites ativos");
    ui->sinal_l->setText("Sinal (db)");
    ui->velocidade_l->setText("Velocidade (Km/h)");
    ui->abrir_bot->setText("Abrir COM");
    ui->menuIdioma->setTitle("Idioma");
    ui->actionIngles->setText("Inglês");
    ui->actionPortugu_s->setText("Português");

}


