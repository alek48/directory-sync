# directory-sync

Projekt na przedmiot SK2.

Program który dba o to, by zawartość katalogu u wszystkich klientów była taka sama. Zmiany są wykrywane natychmiastowo. Współbieżne zmiany są wykrywane i właściwie obsłużone.

Centralny serwer, do którego łączą się klienci. Ścieżka do katalogu podawana jako argument. Aktualizacja powinna być wykonywana po każdej zmianie zawartości u dowolnego klienta (sposób wykrywania zmian do ustalenia; hash value?).
