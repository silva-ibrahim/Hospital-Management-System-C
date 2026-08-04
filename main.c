#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

struct Patient
{
    char firstName[20];
    char lastName[20];
    long long nationalId;
    int age;
    char gender[10];
    char phone[15];
    char appointmentDate[20];
    char appointmentTime[10];
};

struct Doctor
{
    int doctorId;
    char department[20];
    char fullName[30];
    struct Patient appointmentList[5];
    int activeAppointmentCount;
} doctors[10];

void mainMenu();
void createAppointment();
void changeAppointment();
void displayAppointments();
void completeAppointment();
void searchMenu();
void searchByDoctor();
void searchByLastName();
void searchByDate();
void showStatistics();
int checkPatient(long long);
int checkTimeConflict(int doctorIndex, char* timeStr);
void loadData();
void showPatientHistory();

int main(int argc, char *argv[])
{
    loadData();
    mainMenu();

    return 0;
}

void mainMenu()
{
    int choice;

    do
    {
        printf("\n========== HOSPITAL MANAGEMENT SYSTEM ==========\n");
        printf("\nMAIN MENU:\n");
        printf("1. Create New Appointment\n");
        printf("2. Change Doctor\n");
        printf("3. Display Active Appointments\n");
        printf("4. Complete Appointment\n");
        printf("5. Search\n");
        printf("6. Statistics\n");
        printf("7. Patient History\n");
        printf("8. Exit\n");
        printf("Select an operation (1-8): ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice)
        {
            case 1:
                createAppointment();
                break;
            case 2:
                changeAppointment();
                break;
            case 3:
                displayAppointments();
                break;
            case 4: 
                completeAppointment();
                break;
            case 5:
                searchMenu();
                break;
            case 6:
                showStatistics();
                break;
            case 7:
                showPatientHistory();
                break;
            case 8:
                printf("Exiting program. Have a nice day!\n");
                break;
            default:
                printf("Choice must be between 1 and 8!\n");
        }

    } while (choice != 8);
}

void createAppointment()
{
    int k, doctorId;
    long long nationalId;
    int i, emptySlot = -1;

    printf("\n--- DOCTOR / DEPARTMENT LIST ---\n");
    for(k = 0; k < 10; k++) {
        printf("ID: %2d -> Dr. %-20s (%-12s) | Booked: %d/5\n", 
            doctors[k].doctorId, 
            doctors[k].fullName, 
            doctors[k].department, 
            doctors[k].activeAppointmentCount);
    }
    printf("------------------------------------------------------------------\n");

    printf("Enter Patient's National ID: ");
    if (scanf("%lld", &nationalId) != 1)
    {
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (checkPatient(nationalId) == 1)
    {
        printf("[ERROR]: An active appointment already exists in the system with this National ID!\n");
        return;
    }

    printf("Enter Doctor ID for Appointment (1-10): ");
    if (scanf("%d", &doctorId) != 1)
    { 
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (doctorId < 1 || doctorId > 10)
    {
        printf("[ERROR]: Invalid Doctor ID!\n");
        return;
    }

    int doctorIndex = doctorId - 1;

    if (doctors[doctorIndex].activeAppointmentCount >= 5)
    {
        printf("[ERROR]: Selected doctor's appointment capacity is fully booked (Max 5)!\n");
        return;
    }

    for (i = 0; i < 5; i++)
    {
        if (doctors[doctorIndex].appointmentList[i].nationalId == 0)
        {
            emptySlot = i;
            break;
        }
    }

    if (emptySlot == -1)
        return;

    printf("Patient's First Name: ");
    scanf("%s", doctors[doctorIndex].appointmentList[emptySlot].firstName);
    while (getchar() != '\n');

    printf("Patient's Last Name: ");
    scanf("%s", doctors[doctorIndex].appointmentList[emptySlot].lastName);
    while (getchar() != '\n');

    printf("Patient's Age: ");
    scanf("%d", &doctors[doctorIndex].appointmentList[emptySlot].age);
    while (getchar() != '\n');

    printf("Patient's Gender (Female/Male): ");
    scanf("%s", doctors[doctorIndex].appointmentList[emptySlot].gender);
    while (getchar() != '\n');

    printf("Phone Number: ");
    scanf("%s", doctors[doctorIndex].appointmentList[emptySlot].phone);
    while (getchar() != '\n');

    printf("Appointment Date (DD.MM.YYYY): ");
    scanf("%s", doctors[doctorIndex].appointmentList[emptySlot].appointmentDate);
    while (getchar() != '\n');

    char tempTime[10];
    printf("Appointment Time (Ex: 14:30): ");
    scanf("%s", tempTime);
    while (getchar() != '\n');

    if (checkTimeConflict(doctorIndex, tempTime))
    {
        printf("[ERROR]: The doctor already has another appointment at this time! Please choose another time.\n");
        return;
    }

    doctors[doctorIndex].appointmentList[emptySlot].nationalId = nationalId;
    strcpy(doctors[doctorIndex].appointmentList[emptySlot].appointmentTime, tempTime);
    doctors[doctorIndex].activeAppointmentCount++;

    FILE *file = fopen("hospital_data.dat", "wb");
    if (file == NULL)
    {
        perror("[ERROR]: File could not be opened");
        return;
    }
    fwrite(doctors, sizeof(doctors), 1, file);
    fclose(file);

    printf("[SUCCESS]: Appointment has been created successfully.\n");
    printf("Press Enter to continue...");
    getchar();
}

int checkTimeConflict(int doctorIndex, char* timeStr)
{
    int j;
    for (j = 0; j < 5; j++)
    {
        if (doctors[doctorIndex].appointmentList[j].nationalId != 0)
        {
            if (strcmp(doctors[doctorIndex].appointmentList[j].appointmentTime, timeStr) == 0)
            {
                return 1;
            }
        }
    }
    return 0;
}

void changeAppointment()
{
    long long nationalId;
    int newDoctorId, i, j;
    int oldDoctorIndex = -1, oldSlotIndex = -1;

    printf("\nEnter National ID of Patient to Change Department: ");
    if (scanf("%lld", &nationalId) != 1)
    {
        while(getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (doctors[i].appointmentList[j].nationalId == nationalId)
            {
                oldDoctorIndex = i;
                oldSlotIndex = j;
                break;
            }
        }
        if (oldDoctorIndex != -1)
            break;
    }

    if (oldDoctorIndex == -1)
    {
        printf("[ERROR]: No active appointment found registered with this National ID!\n");
        return;
    }

    printf("New Doctor ID to Transfer (1-10): ");
    if (scanf("%d", &newDoctorId) != 1)
    {
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (newDoctorId < 1 || newDoctorId > 10)
    {
        printf("[ERROR]: Invalid Doctor ID!\n");
        return;
    }

    int newIndex = newDoctorId - 1;

    if (doctors[newIndex].activeAppointmentCount >= 5)
    {
        printf("[ERROR]: Target doctor's appointment capacity is full!\n");
        return;
    }

    int newSlotIndex = -1;
    for (j = 0; j < 5; j++)
    {
        if (doctors[newIndex].appointmentList[j].nationalId == 0)
        {
            newSlotIndex = j;
            break;
        }
    }

    doctors[newIndex].appointmentList[newSlotIndex] = doctors[oldDoctorIndex].appointmentList[oldSlotIndex];
    doctors[newIndex].activeAppointmentCount++;

    doctors[oldDoctorIndex].appointmentList[oldSlotIndex].nationalId = 0;
    strcpy(doctors[oldDoctorIndex].appointmentList[oldSlotIndex].firstName, "");
    strcpy(doctors[oldDoctorIndex].appointmentList[oldSlotIndex].lastName, "");
    strcpy(doctors[oldDoctorIndex].appointmentList[oldSlotIndex].appointmentTime, "");
    doctors[oldDoctorIndex].activeAppointmentCount--;

    FILE *file = fopen("hospital_data.dat", "wb");
    if (file != NULL)
    {
        fwrite(doctors, sizeof(doctors), 1, file);
        fclose(file);
    }

    printf("[SUCCESS]: Doctor change completed!\n");
    printf("Press Enter to continue...");
    getchar();
}

void displayAppointments()
{
    int i, j, isEmpty = 1;
    printf("\n================ ACTIVE APPOINTMENTS LIST ================\n");
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (doctors[i].appointmentList[j].nationalId != 0)
            {
                printf("Doctor: Dr. %s (%s)\n", doctors[i].fullName, doctors[i].department);
                printf("  -> Patient: %s %s (National ID: %lld)\n", 
                    doctors[i].appointmentList[j].firstName, 
                    doctors[i].appointmentList[j].lastName, 
                    doctors[i].appointmentList[j].nationalId);
                printf("  -> Age: %d | Gender: %s | Phone: %s\n", 
                    doctors[i].appointmentList[j].age, 
                    doctors[i].appointmentList[j].gender, 
                    doctors[i].appointmentList[j].phone);
                printf("  -> Appointment Date/Time: %s - %s\n\n", 
                    doctors[i].appointmentList[j].appointmentDate, 
                    doctors[i].appointmentList[j].appointmentTime);
                isEmpty = 0;
            }
        }
    }
    if (isEmpty == 1)
        printf("There are no patients with active appointments in the system.\n");
    
    printf("Press Enter to return to main menu...");
    getchar();
}

void completeAppointment()
{
    long long nationalId;
    int i, j, found = 0;

    printf("\nNational ID of Patient Whose Examination is Completed: ");
    if (scanf("%lld", &nationalId) != 1)
    {
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (doctors[i].appointmentList[j].nationalId == nationalId)
            {
                FILE *archive = fopen("hospital_archive.txt", "a");
                if (archive != NULL)
                {
                    time_t t = time(NULL);
                    struct tm tm1 = *localtime(&t);

                    fprintf(archive, "%lld %s %s %d %s %s %s %s %02d.%02d.%d\n", 
                        doctors[i].appointmentList[j].nationalId, 
                        doctors[i].appointmentList[j].firstName, 
                        doctors[i].appointmentList[j].lastName, 
                        doctors[i].doctorId, 
                        doctors[i].department,
                        doctors[i].appointmentList[j].appointmentDate,
                        doctors[i].appointmentList[j].appointmentTime,
                        doctors[i].appointmentList[j].phone,
                        tm1.tm_mday, tm1.tm_mon + 1, tm1.tm_year + 1900);
                    fclose(archive);
                }

                doctors[i].appointmentList[j].nationalId = 0;
                strcpy(doctors[i].appointmentList[j].firstName, "");
                strcpy(doctors[i].appointmentList[j].lastName, "");
                strcpy(doctors[i].appointmentList[j].appointmentTime, "");
                doctors[i].activeAppointmentCount--;

                found = 1;
                break;
            }
        }
        if (found) break;
    }

    if (found == 0)
    {
        printf("[ERROR]: No active appointment matching this National ID found!\n");
        return;
    }

    FILE *file = fopen("hospital_data.dat", "wb");
    if (file != NULL)
    {
        fwrite(doctors, sizeof(doctors), 1, file);
        fclose(file);
    }

    printf("[SUCCESS]: Examination completed, patient archived and appointment closed!\n");
    printf("Press Enter to continue...");
    getchar();
}

void searchMenu()
{
    int choice;
    do
    {
        printf("\n--- ADVANCED SEARCH MENU ---\n");
        printf("1- Search by Doctor\n");
        printf("2- Search by Patient Last Name\n");
        printf("3- Search by Date\n");
        printf("4- Return to Upper Menu\n");
        printf("Your Choice: ");
        if (scanf("%d", &choice) != 1) { while (getchar() != '\n'); continue; }
        while (getchar() != '\n');

        if (choice == 1)
		    searchByDoctor();
        else if
    		(choice == 2) searchByLastName();
        else if (choice == 3)
    		searchByDate();

    } while (choice != 4);
}

void searchByDoctor()
{
    int id, j, exists = 0;
    printf("Doctor ID to Search (1-10): ");
    if (scanf("%d", &id) != 1)
    {
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    if (id < 1 || id > 10)
        return;
    int index = id - 1;

    printf("\n--- Dr. %s (%s) Appointments ---\n", doctors[index].fullName, doctors[index].department);
    for (j = 0; j < 5; j++)
    {
        if (doctors[index].appointmentList[j].nationalId != 0)
        {
            printf("Patient: %s %s | Phone: %s | Time: %s\n", 
                doctors[index].appointmentList[j].firstName,
                doctors[index].appointmentList[j].lastName,
                doctors[index].appointmentList[j].phone,
                doctors[index].appointmentList[j].appointmentTime);
            exists = 1;
        }
    }
    if (!exists) printf("This doctor has no active appointments.\n");
    printf("\nPress Enter to continue...");
    getchar();
}

void searchByLastName()
{
    char lastName[20];
    int i, j, exists = 0;
    printf("Patient Last Name to Search: ");
    scanf("%s", lastName);
    while (getchar() != '\n');

    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (doctors[i].appointmentList[j].nationalId != 0)
            {
                if (strcmp(doctors[i].appointmentList[j].lastName, lastName) == 0)
                {
                    printf("Found -> Dr. %s | Patient: %s %s | Phone: %s | Date: %s %s\n", 
                        doctors[i].fullName,
                        doctors[i].appointmentList[j].firstName,
                        doctors[i].appointmentList[j].lastName,
                        doctors[i].appointmentList[j].phone,
                        doctors[i].appointmentList[j].appointmentDate,
                        doctors[i].appointmentList[j].appointmentTime);
                    exists = 1;
                }
            }
        }
    }
    if (!exists) printf("No records found matching this last name.\n");
    printf("\nPress Enter to continue...");
    getchar();
}

void searchByDate()
{
    char dateStr[20];
    int i, j, exists = 0;
    printf("Date to Search (DD.MM.YYYY): ");
    scanf("%s", dateStr);
    while (getchar() != '\n');

    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (doctors[i].appointmentList[j].nationalId != 0)
            {
                if (strcmp(doctors[i].appointmentList[j].appointmentDate, dateStr) == 0)
                {
                    printf("Date Match -> Dr. %s | Patient: %s %s | Time: %s\n", 
                        doctors[i].fullName,
                        doctors[i].appointmentList[j].firstName,
                        doctors[i].appointmentList[j].lastName,
                        doctors[i].appointmentList[j].appointmentTime);
                    exists = 1;
                }
            }
        }
    }
    if (!exists)
        printf("No appointments found on this date.\n");
    printf("\nPress Enter to continue...");
    getchar();
}

void showStatistics()
{
    int totalAppointments = 0;
    int maxAppointments = -1;
    int minAppointments = 6;
    int busiestIndex = -1;
    int mostAvailableIndex = -1;
    int i;
    
    for (i = 0; i < 10; i++)
    {
        totalAppointments += doctors[i].activeAppointmentCount;
        
        if (doctors[i].activeAppointmentCount > maxAppointments)
        {
            maxAppointments = doctors[i].activeAppointmentCount;
            busiestIndex = i;
        }
        if (doctors[i].activeAppointmentCount < minAppointments)
        {
            minAppointments = doctors[i].activeAppointmentCount;
            mostAvailableIndex = i;
        }
    }

    printf("\n================ HOSPITAL STATISTICS ================\n");
    printf("Total Active Patient Count: %d / 50\n", totalAppointments);
    printf("\nHospital General Occupancy Rate: %% %.2f\n", ((float)totalAppointments / 50) * 100);

    if (totalAppointments == 0)
    {
        printf("Busiest Doctor: No doctors with appointments yet.\n");
        printf("Most Available Doctor: All doctors available (0 Appointments).\n");
    }
    else
    {
        printf("Busiest Doctor: Dr. %s (%s) - %d Appointments\n", 
            doctors[busiestIndex].fullName, 
            doctors[busiestIndex].department, maxAppointments);
        printf("Most Available Doctor: Dr. %s (%s) - %d Appointments\n", 
            doctors[mostAvailableIndex].fullName, 
            doctors[mostAvailableIndex].department, minAppointments);
    }
    
    printf("========================================================\n");
    printf("Press Enter to return to main menu...");
    while (getchar() != '\n');
    getchar();
}

int checkPatient(long long nationalId)
{
    int i, j;
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 5; j++)
        {
            if (doctors[i].appointmentList[j].nationalId == nationalId)
                return 1;
        }
    }
    return 0;
}

void loadData()
{
    FILE *file = fopen("hospital_data.dat", "rb");
    
    if (file == NULL)
    {
        file = fopen("hospital_data.dat", "wb");
        char *departments[10] = {"Cardiology", "Neurology", "Orthopedics", "Internal Medicine", "Ophthalmology", "ENT", "Dermatology", "Psychiatry", "Urology", "Pediatrics"};
        char *doctorNames[10] = {"Alexander Smith", "Emily Johnson", "Michael Brown", "Sarah Davis", "William Miller", "Jessica Wilson", "Daniel Moore", "Laura Taylor", "David Anderson", "Sophia Thomas"};
        int i, j;
        
        for (i = 0; i < 10; i++)
        {
            doctors[i].doctorId = i + 1;
            strcpy(doctors[i].department, departments[i]);
            strcpy(doctors[i].fullName, doctorNames[i]);
            doctors[i].activeAppointmentCount = 0;
            
            for (j = 0; j < 5; j++)
            {
                doctors[i].appointmentList[j].nationalId = 0;
                strcpy(doctors[i].appointmentList[j].appointmentTime, "");
            }
        }
        
        fwrite(doctors, sizeof(doctors), 1, file);
        fclose(file);

        FILE *archive = fopen("hospital_archive.txt", "w");
        if (archive != NULL) fclose(archive);

        printf("New database file created.\n");
    }
    else
    {
        fread(doctors, sizeof(doctors), 1, file);
        fclose(file);
        
        int i, j;
        for (i = 0; i < 10; i++)
        {
            for (j = 0; j < 5; j++)
            {
                if (doctors[i].appointmentList[j].nationalId == 0)
                {
                    strcpy(doctors[i].appointmentList[j].appointmentTime, "");
                }
            }
        }
        printf("Hospital data loaded successfully.\n");
    }
}

void showPatientHistory()
{
    FILE *archive = fopen("hospital_archive.txt", "r");
    if (archive == NULL)
    {
        printf("\nNo past archive records found yet!\n");
        printf("Press Enter to return to main menu...");
        while (getchar() != '\n');
        getchar();
        return;
    }

    fseek(archive, 0, SEEK_END);
    long fileSize = ftell(archive);
    if (fileSize == 0)
    {
        printf("\n================ PAST PATIENT ARCHIVE ================\n");
        printf("No past archive records found yet!\n");
        printf("=====================================================\n");
        fclose(archive);
        printf("Press Enter to return to main menu...");
        while (getchar() != '\n');
        getchar();
        return;
    }

    rewind(archive);
    char line[256];

    printf("\n================ PAST PATIENT ARCHIVE ================\n");
    while (fgets(line, sizeof(line), archive) != NULL)
    {
        printf("%s", line);
    }
    printf("=====================================================\n");
    fclose(archive);

    printf("Press Enter to return to main menu...");
    getchar();
}
