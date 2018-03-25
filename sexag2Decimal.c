/*
 * This should be fairly self-explanatory at this stage of the course...
 *  INPUT:   the pointer to the "Degree String"
 *  RETURNS: valid floating degree back, 0.0 if errors in string.
 */
float sexag2decimal(char *degString){
	if (degString == NULL || *degString == '\0'){
		return 0.0;
	}

	int i=0, dash=0, masFlag=0, div=10, sign=1;
	float ddd=0, mm=0, ss=0;

	while(*(degString+i)!='\0'){
		if (*(degString+i) == '-' ){
			dash++;
		}
		else if(*(degString + i) == '.'){
			masFlag = 1;
		}
		// No change to sign if North or East
		else if(*(degString+i) == 'N' || *(degString+i) == 'E'){
			sign = 1;
		}
		// Sign becomes negative if South or West
		else if(*(degString+i) == 'S' || *(degString+i) == 'W'){
			sign = -1;
		}
		else{
			switch(dash){
			case 0:
				ddd*=10;
				ddd+=(float) *(degString + i) - '0';
				break;
			case 1:
				mm*=10;
				mm+=(float) *(degString + i) - '0';
				break;
			case 2:
				if(masFlag==0){
					ss*=10;
					ss+=(float) *(degString + i) - '0';
					break;
				}
				else{
					ss+=((float) *(degString + i) - '0')/div;
					div*=10;
					break;
				}
			}
		}

		i++;
	}
	// Return 0.0 if out of bounds.
	return (ddd>180 || mm>=60 || ss>=60 || ddd<0 || mm<0 || ss<0) ? 0.0 : sign*(ddd + mm/60 + ss/3600);

}
