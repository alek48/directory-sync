# directory-sync

Projekt na przedmiot SK2.

Program który dba o to, by zawartość katalogu u wszystkich klientów była taka sama. Zmiany są wykrywane natychmiastowo. Współbieżne zmiany są wykrywane i właściwie obsłużone.

# Opis

~~Uruchamiany jest centralny serwer, który dba o synchronizację plików. Jeden proces-serwer umożliwia synchronizację tylko jednego katalogu.~~

Uruchamiany jest centralny serwer, który dba o synchronizację plików. Jeden proces-serwer umożliwia synchronizację wieku katalogów.

Każdy klient łączy się z serwerem; jako argumenty przy uruchamianiu podawany jest adres serwera oraz lokalna ścieżka do katalogu, który ma być współdzielony.

~~Zawartość katalogu pierwszego klienta ustala jego "stan początkowy".~~

~~Natychmiast po połączeniu, katalog klienta zostaje nadpisany przez "wersję serwerową".~~

~~Po każdej zmianie zawartości, klient wysyła do serwera nową wersję pliku, którą serwer rozsyła do wszystkich innych połączonych klientów.~~

~~W przypadku, gdy wielu klientów próbuje jednocześnie aktualizować ten sam plik, priorytetowo traktowany jest ten, którego informacja jako pierwsza została dostarczona do serwera. Wszystkie inne strony konfliktu zostają poinformowane i Użytkownik wybiera, czy przyjmuje wersję od serwera, czy wysyła (i efektywnie nadpisuje) do serwera swoją wersję pliku.~~

~~Jedną z opcji konfiguracji serwera jest "okno kolizji", i.e. maksymalny odstęp czasu pomiędzy otrzymaniem różnych wersji tego samego pliku, który jest interpretowany jako kolizja.~~

~~Przy uruchamianiu serwera można ustawić maksymalny dopuszczalny rozmiar katalogu oraz maksymalny dopuszczony rozmiar pojedynczego pliku. W przypadku limitów, użytkownik wysyłający zostaje poinformowany o problemie, a zmiana nie jest synchronizowana.~~

Obsługa synchronizacji następuje po stronie klienta z poziomu CLI. Po połączeniu z serwerem użytkownik wybiera katalog ("VAULT"), do którego chce się synchronizować.

Po połączeniu z vault'em, klient może wybierać pliki do wysłania zub ściągnięcia z serwera.
