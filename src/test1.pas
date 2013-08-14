programa carne_de_perro;
inicio
  procedimiento A (uno_p: entero); inicio
	    variables
	      dos: entero;
	    finvar;

	    procedimiento B(dos_p: entero); inicio
		  variables
		    tres: entero;
		  finvar;

		  procedimiento C(tres_p: entero); inicio
			  variables
			    cuatro: entero;
			  finvar;
			  cuatro := tres_p + 200;
			  salida "Procedimiento C ", cuatro, "\n";
		  fin

		  tres := dos_p + 10;
		  salida "procedimiento B ", tres, "\n";
		  C(tres);
	    fin

	    dos:= uno_p + 1;
	    salida "procedimiento A ", dos, "\n";
	    B(dos);

  fin
  salida "carne_de_perro ", 0, "\n";
  A(0);

  fin.






