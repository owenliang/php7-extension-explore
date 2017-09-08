<?php

// default exception 
try {
    test_exception(0);
} catch (Exception $e) {
    var_dump($e);
}

// self-defined exception
try {
    test_exception(1);
} catch (Exception $e) {
    var_dump($e);
}

test_error();

echo "fatal error will cause script stop, so never reach here";
