function Student(firstName, lastName, studentID) {
    this.firstName = firstName;
    this.lastName = lastName;
    this.studentID = studentID;
    this.display = function() {
        console.log(this.firstName + " " + this.lastName + " " + this.studentID);
    };
}

// Create an array of new students.
var student1 = new Student("Himanshu", "Jain", "123");
var student2 = new Student("Aditya", "Manjunath", "456");
var students = [student1, student2];

// Add a 'graduated' property to just one of your students.
student1.graduated = true;

// Now create another student without using the constructor. (use the object literal {} syntax).
// Set the prototype chain manually using the proto field.
// Make sure the display method still works (without you having to add it to the object explicitly).
var student3 = {
    firstName: "Nitish",
    lastName: "Gridhar",
    studentID: "789",
    __proto__: new Student() // Inherit from an instance so we get the display() method created in the constructor
};

// Invoke display to test
student1.display();
student2.display();
student3.display();
