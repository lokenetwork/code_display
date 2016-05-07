<?php
/**
 * Created by PhpStorm.
 * User: loken
 * Date: 5/5/16
 * Time: 7:49 PM
 */
$fp = fopen("./number_count", "r");
if($fp){
  $current_number = intval(fgets($fp));
  //var_dump($current_number);
}else{
  file_put_contents("./log","open file fail\n");
}
fclose($fp);

$new_number = $current_number + 1;
$w_fp = fopen("./number_count", "w+");
if(flock($w_fp, LOCK_EX)){
  $flag = fwrite($w_fp, "{$new_number}");
  if(!$flag){
    file_put_contents("./log","write file fail\n");
  }
}else{
  file_put_contents("./log","Error locking file!\n");
}
fclose($w_fp);




