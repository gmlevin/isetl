   make_account :=
    func(name, balance);
	return 
	{
	    ["deposit", func(n); balance := balance + n; end],
	    ["withdraw", func(n); balance := balance - n; end],
	    ["balance", func(); return balance; end],
	    ["name", func(); return name; end]
	};
    end;

   gary := make_account("Gary Levin", 1000);
   carol := make_account("Carol Simon Levin", 1000);
gary("name")();
carol("name")();
gary("deposit")(100);
carol("withdraw")(300);
gary("balance")();
carol("balance")();
