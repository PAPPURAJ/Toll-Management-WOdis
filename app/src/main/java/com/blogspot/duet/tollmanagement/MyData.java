package com.blogspot.duet.tollmanagement;

public class MyData {
    public String Balance, Count;
     public MyData() {
    }

    public MyData(String balance, String count) {
        Balance = balance;
        Count = count;
    }

    public String getBalance() {
        return Balance;
    }

    public String getCount() {
        return Count;
    }
}
