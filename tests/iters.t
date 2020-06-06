is_sym := func(R);
		return forall [x,y] in R | [y,x] in R;
	  end;

lt := {[i,j] : i,j in [1..5] | i < j};

lt;

is_sym(lt);

mod3 := {[i,j] : i,j in [1..5] | (i-j) mod 3 = 0};

mod3;

is_sym(mod3);
