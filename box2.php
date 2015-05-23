<?php
if($fp = fsockopen("localhost", 3002, $errno, $errstr, 1)) // открываем порт, в качестве посредника между роутером и ардуиной выступает ser2net
   { 
      fwrite($fp, 'Y+=Z'); // отправляем в порт дескриптор Y+=Z
      fwrite($fp, $_GET['df']); // отправляем в порт символ полученый от html странички
      stream_set_timeout($fp, 0, 150000); // полезный таймаут, если ответа нет, то поток закроется через 150 мс
      $bufft = fgets($fp); // получаем ответ от ардуины
      fclose($fp); // закрываем порт
      echo $bufft; // отправляем ответ клиенту 
   } 

else
   { 
      usleep(70000);
      $fp = fsockopen("localhost", 3002, $errno, $errstr, 1);
      fwrite($fp, 'Y+=Z'); 
      fwrite($fp, $_GET['df']);
      stream_set_timeout($fp, 0, 150000);
      $bufft = fgets($fp);
      fclose($fp);
      echo $bufft;
   }
?> 








