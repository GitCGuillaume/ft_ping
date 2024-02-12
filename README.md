# ft_ping

Ce projet a besoin des droits administrateurs.

Ping est une commande qui permet de vérifier l'accessibilité vers une machine dans un réseau, pour ce faire, elle utilise le protocole de communication ICMP. ([Internet Control Message Protocol](https://fr.wikipedia.org/wiki/Internet_Control_Message_Protocol))

Commandes pour compiler ping:
```
make all
make bonus
```

```
Pour lancer la commande, utiliser sudo ./ft_ping ou devenir administrateur avec su
sudo ./ft_ping google.com
sudo ./ft_ping_bonus google.com
```

Liste d'options

-v ou --verbose (sudo ./ft_ping -v google.com)
>Affiche des statistiques en plus
-? ou --help (sudo ./ft_ping --help)
>Liste tous les options disponibles (Texte de base de la commande ping du pack inetutils)
--usage (sudo ./ft_ping --usage)
>Affiche la liste des commandes de la commande ping du pack inetutils

Liste d'options bonus

--ttl (sudo ./ft_ping_bonus -v --ttl 1 google.com)
>Modifier la valeur de base Time-to-live lors de la transmission du paquet
-w ou --timeout (sudo ./ft_ping_bonus -w 1 google.com)
>S'arrête après X seconds
-l ou --preload (sudo ./ft_ping_bonus -l 5 google.com)
>Envoi le nombre de paquets aussi rapidement que possible
-p ou --pattern (sudo ./ft_ping_bonus -p 123 google.com)
>Modifie les valeurs du paquet, cela est utile pour diagnostiquer les problèmes dans le réseau
-T ou --tos (sudo ./ft_ping_bonus T 1 google.com)
>Règle la transmission de priorité paquet
