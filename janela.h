#ifndef JANELA_H
#define JANELA_H

#define _USE_MATH_DEFINES

#include <QMainWindow>
#include <QDebug>
#include <vector>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <cmath>
#include <map>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QQueue>


using namespace std;

namespace Ui {
class Janela;
}

class Janela : public QMainWindow
{
    Q_OBJECT

public:
    explicit Janela(QWidget *parent = 0);
    ~Janela();

private slots:

    void sat_but_clicked();

    void readSerial();

    void on_actionIngles_triggered();

    void on_actionPortugu_s_triggered();

    void on_abrir_bot_clicked();

    void on_fechar_bot_clicked();

    void on_refresh_bot_clicked();

    void on_mapa_bot_clicked();

private:

    struct gpgga_data{ //Global Positioning System Fix Data
        QString A_hora;
       QString B_latitude;
         char C_latitude_dir;
       QString D_longitude;
         char E_longitude_dir;
          int F_qualidade_fixa= -1;
          int G_quantidade_satelites = -1;
        float H_diluicao_da_precisao_horizontal = -1;
        float I_altitude = -1;
         char J_unidade_medida_altura;
        float K_altura_geoide_acima_elipsoide = -1;
         char L_unidade_medida_geoide;
        float M_idade_ultima_atualizacao_dgps = -1;
          int N_dgps_id = -1;
    };


    struct gpgsa_data{ //GPS DOP and active satellites
         char A_modo=-1;
         char B_modo_dimensional=-1;
        int  *CN_ids_satelites;
        float O_PDOP=-1;
        float P_HDOP=-1;
        float Q_VDOP=-1;
    };

    struct satelite{
        int A_sv_prn=-1;
        int B_elevacao_graus=-1; //90 max
        int C_azimuth=-1;
        int D_snr=-1;
    };

    struct gpgsv_data{ //GPS Satellites in view
             unsigned A_total_mensagens=-1;
             unsigned B_numero_desta_mensagem=-1;
             int C_satelites_visiveis=-1;
             satelite DS_sats[4];
    };

    struct gprmc_data{ //Recommended minimum specific GPS/Transit data
            int A_hora=-1;
           char B_warning=-1;
          float C_latitude=-1;
           char D_latitude_coord=-1;
          float E_longitude=-1;
           char F_longitude_coord=-1;
          float G_velocidade_nos=-1;
          float H_CMG=-1;
            int I_data=-1;
          float J_variacao_magnetica=-1;
           char K_variacao_magnetica_coord=-1;


    };

    unsigned pacotes_invalidos=0;
    void decodifica(QStringList pacotes);

    void desenha_sat(float raio, float azimuth, bool usado,int num);
    void limpa_sat();

    void refresh_com();

    void GPGGA(QString pacote);
    void GPGSV(QString pacote);
    void GPGSA(QString pacote);
    void GPRMC(QString pacote);

    bool checksum(QString pacote);

    vector<satelite *> satelites_v;
    vector<int> satelites_a;
    vector<QVBoxLayout *> layouts;
    vector<QPushButton *> sat_botoes;

    QGraphicsScene *scene;
    vector<QGraphicsEllipseItem *> sats_grap;

    QBrush *brush_branco;
    QBrush *brush_amarelo;
    QBrush *brush_preto;
    QBrush *brush_azul;
    QPen *pen_preto;
    QMap<int, int> sat_ref;

    QSerialPort *com;
    QByteArray serialData;
    QString serialBuffer;


    Ui::Janela *ui;
};

#endif // JANELA_H
