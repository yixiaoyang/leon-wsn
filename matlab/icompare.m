function  res = icompare(a1,a2,b1,b2)
if(abs(a1-a2) > abs(b1-b2))
	res = 1;
elseif(abs(a1-a2) < abs(b1-b2))
	res = -1;
else
	res = 0;
end
