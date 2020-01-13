# ubuntu 18.04 谷歌浏览器enter password to unlock your login keyring

![unlock](./assets/unlock.png)

``` {.line-numbers highlight=[1, 3]}
hero@hero:~$ find ~/ -name login.keyring
/home/hero/.local/share/keyrings/login.keyring
hero@hero:~$ sudo rm -rf /home/hero/.local/share/keyrings/login.keyring
[sudo] password for hero:
hero@hero:~$
```

> 再次打开谷歌浏览器
> 提示设置密码,设置一下即可,之后应该不会再弹出这种提示了.
