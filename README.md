# directory-sync

Projekt na przedmiot SK2.

Program który dba o to, by zawartość katalogu u wszystkich klientów była taka sama. Zmiany są wykrywane natychmiastowo. Współbieżne zmiany są wykrywane i właściwie obsłużone.

# Opis

Uruchamiany jest centralny serwer, który dba o synchronizację plików. Jeden proces-serwer umożliwia synhronizację tylko jednego katalogu.

Każdy klient łączy się z serwerem; jako argumenty przy uruchamianiu podawany jest adres serwera oraz ścieżka do katalogu, który ma być współdzielony.

Zawartość katalogu pierwszego klienta ustala jego "stan początkowy".

Natychmiast po połączeniu, katalog klienta zostaje nadpisany przez "wersję serwerową".

Po każdej zmianie zawartości, klient wysyła do serwera nową wersję pliku, którą serwer rozsyła do wszystkich innych połączonych klientów.

W przypadku, gdy wielu klientów próbuje jednocześnie aktualizować ten sam plik, priorytetowo traktowany jest ten, którego informacja jako pierwsza została dostarczona do serwera. Wszystkie inne strony konfliktu zostają poinformowane i Użytkownik wybiera, czy przyjmuje wersję od serwera, czy wysyła (i efektywnie nadpisuje) do serwera swoją wersję pliku.

Jedną z opcji konfiguracji serwera jest "okno kolizji", i.e. maksymalny odstęp czasu pomiędzy otrzymaniem różnych wersji tego samego pliku, który jest interpretowany jako kolizja.

Przy uruchamianiu serwera można ustawić maksymalny dopuszczalny rozmiar katalogu oraz maksymalny dopuszczony rozmiar pojedynczego pliku. W przypadku limitów, użytkownik wysyłający zostaje poinformowany o problemie, a zmiana nie jest synchronizowana.
