<?php

// parent
$obj = new MyExt();

echo $obj->strtolower("HELLO") . $obj->strtoupper("php") . $obj->strtolower(2017) . PHP_EOL;
echo $obj->version() . PHP_EOL;
echo MyExt::$author . PHP_EOL;
echo MyExt::BUILD_DATE . PHP_EOL;

echo "====================" . PHP_EOL;

// child
$obj = new MyExt_Child();

echo $obj->strtolower("HELLO") . $obj->strtoupper("php") . $obj->strtolower(2017) . PHP_EOL;
echo $obj->version() . PHP_EOL;
echo MyExt_Child::$author . PHP_EOL;
echo MyExt_Child::BUILD_DATE . PHP_EOL;

// global function
test_object();

test_array();

test_resource();

$file = __DIR__ . "/other.php";
var_dump(test_include($file)); // array
var_dump(test_include($file)); // false
var_dump($file);

echo SOME_DEF . PHP_EOL;

echo GITHUB . PHP_EOL;
