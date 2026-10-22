import serial
import time
import csv
import os

# --- AYARLAR ---
# Kendi bilgisayarındaki porta göre burayı değiştireceksin.
# Windows için örneğin: 'COM3' veya 'COM4'
# Mac/Linux için örneğin: '/dev/ttyUSB0' veya '/dev/cu.usbserial-0001'
SERIAL_PORT = 'COM3'
BAUD_RATE = 115200

# Deney adı (Dosya adını belirler - Her deneyde burayı değiştirmelisin)
# Örnek: "baseline_yuk_yok", "baseline_yuk_yuksek", "timehopping_yuk_yuksek"
EXPERIMENT_NAME = "baseline_test_1"
DURATION_SECONDS = 60  # Deneyin kaç saniye süreceği (Örn: 60 saniye dinle ve dur)

# Dosya yolu ayarı
OUTPUT_DIR = "deney_verileri"
if not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)

csv_filename = f"{OUTPUT_DIR}/{EXPERIMENT_NAME}.csv"


def log_serial_data():
    try:
        # Seri portu aç
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"[{SERIAL_PORT}] portuna bağlanıldı. {BAUD_RATE} baud rate ile dinleniyor...")
        print(f"Deney '{EXPERIMENT_NAME}' başlatıldı. {DURATION_SECONDS} saniye boyunca veri toplanacak.")
        print(f"Veriler şuraya kaydedilecek: {csv_filename}")

        start_time = time.time()

        with open(csv_filename, mode='w', newline='') as csv_file:
            # Sütun başlıklarını (Header) yaz
            csv_writer = csv.writer(csv_file)
            csv_writer.writerow(["RX_TIME", "SEQ_NUM", "TX_TIME", "LEN"])

            while (time.time() - start_time) < DURATION_SECONDS:
                if ser.in_waiting > 0:
                    # Seri porttan gelen satırı oku ve temizle
                    line = ser.readline().decode('utf-8').strip()

                    # Eğer gelen veri bizim beklediğimiz formatta (virgüllü) ise kaydet
                    # Gürültü veya Arduino'nun başlangıç mesajlarını filtrelemek için basit kontrol
                    if len(line.split(',')) == 4:
                        print(f"Alınan Paket: {line}")
                        # Stringi parçala ve CSV'ye yaz
                        data_parts = line.split(',')
                        csv_writer.writerow(data_parts)

        print(f"\n--- Deney Süresi ({DURATION_SECONDS} sn) Doldu ---")
        print(f"Veriler başarıyla kaydedildi: {csv_filename}")

    except serial.SerialException as e:
        print(
            f"Seri port hatası! Portun doğru olduğundan ve başka bir uygulamanın (örn. Arduino Serial Monitor) portu kullanmadığından emin ol.\nHata detayı: {e}")
    except KeyboardInterrupt:
        print("\nKullanıcı tarafından manuel olarak durduruldu.")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("Seri port kapatıldı.")


if __name__ == '__main__':
    log_serial_data()


    #ESP32'ler çalışmaya başladığında, sen bu Python dosyasını çalıştıracaksın.
    # Betik belirlediğin süre boyunca (örn: 60 saniye) alıcıdan gelen paketleri dinleyecek ve ardından kendi kendine durup temiz bir .csv dosyası üretecek.
    # Sen sadece EXPERIMENT_NAME değişkenini (örneğin "baseline_yuksek_trafik" gibi) değiştireceksin.
    # Böylece "tekrarlanabilirlik" başarı kriterimizi tam anlamıyla sağlamış olacağız.
